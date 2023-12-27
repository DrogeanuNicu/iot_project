#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "SysCtrl.h"
#include "WifiCtrl.h"
#include "Mqtt.h"
#include "FanCtrl.h"
#include "PumpCtrl.h"
#include "MoistCtrl.h"
#include "I2cCtrl.h"
#include "LcdCtrl.h"
#include "TimeCtrl.h"
#include "Dht11.h"

#define MAX_MSG_BUF_LEN 90

static const char *TAG = "SysCtrl";
typedef struct
{
    int TempMin;
    int TempMax;
    int HumMin;
    int HumMax;
    int MoistMin;
    int MoistMax;
} SysCtrl_Limits;

static SysCtrl_Limits Limits = {
    .TempMin = 20,
    .TempMax = 25,
    .HumMin = 40,
    .HumMax = 60,
    .MoistMin = 30,
    .MoistMax = 70,
};

static void SetLimits(esp_mqtt_event_handle_t Event);
static void GetLimits();

void SysCtrl_Init(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Init");
#endif
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(WifiCtrl_Init());
    ESP_ERROR_CHECK(FanCtrl_Init());
    ESP_ERROR_CHECK(PumpCtrl_Init());
    ESP_ERROR_CHECK(MoistCtrl_Init());
    ESP_ERROR_CHECK(I2cCtrl_Init());
    ESP_ERROR_CHECK(LcdCtrl_Init());
    ESP_ERROR_CHECK(DHT11_init());
    ESP_ERROR_CHECK(TimeCtrl_Init());
    ESP_ERROR_CHECK(Mqtt_Init());
}

void SysCtrl_Main(void)
{
    uint32_t Timestamp;
    Dht11_Reading Dht11Reading = {
        .status = DHT11_OK,
        .temperature = 0,
        .humidity = 0};
    int MoistReading = 0;
    bool FanState = false;
    bool PumpState = false;
    char MessageBuffer[MAX_MSG_BUF_LEN];

    while (1)
    {
        Timestamp = TimeCtrl_GetTime();
        Dht11Reading = DHT11_read();
        MoistReading = MoistCtrl_GetMoist();
        FanState = FanCtrl_GetState();
        PumpState = PumpCtrl_GetState();

        snprintf(MessageBuffer, MAX_MSG_BUF_LEN, "%10lu,%3d,%3d,%3d,%d,%d%c",
                 Timestamp,
                 Dht11Reading.temperature,
                 Dht11Reading.humidity,
                 MoistReading,
                 FanState ? 1 : 0,
                 PumpState ? 1 : 0,
                 '\0');

#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "%.*s", MAX_MSG_BUF_LEN, MessageBuffer);
#endif

        Mqtt_SendMessage("plant", MessageBuffer, strlen(MessageBuffer));
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void SysCtrl_ProcessEvent(esp_mqtt_event_handle_t Event)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "%.*s", Event->data_len, Event->data);
#endif

    if (Event->data_len < 1)
    {
        ESP_LOGE(TAG, "Invalid command!");
    }
    else if (1 == Event->data_len)
    {
        GetLimits();
    }
    else
    {
        SetLimits(Event);
    }
}

static void SetLimits(esp_mqtt_event_handle_t Event)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Setting the limits: %.*s", Event->data_len, Event->data);
#endif
    char *Token = strtok(Event->data, ",");
    Token = strtok(NULL, ","); /* Skip the command */
    Limits.TempMin = atoi(Token);
    Token = strtok(NULL, ",");
    Limits.TempMax = atoi(Token);
    Token = strtok(NULL, ",");
    Limits.HumMin = atoi(Token);
    Token = strtok(NULL, ",");
    Limits.HumMax = atoi(Token);
    Token = strtok(NULL, ",");
    Limits.MoistMin = atoi(Token);
    Token = strtok(NULL, ",");
    Limits.MoistMax = atoi(Token);
}

static void GetLimits()
{
    char MessageBuffer[MAX_MSG_BUF_LEN];
    snprintf(MessageBuffer, MAX_MSG_BUF_LEN, "%3d,%3d,%3d,%3d,%3d,%3d%c",
             Limits.TempMin,
             Limits.TempMax,
             Limits.HumMin,
             Limits.HumMax,
             Limits.MoistMin,
             Limits.MoistMax,
             '\0');

#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Getting the limits: %.*s", strlen(MessageBuffer), MessageBuffer);
#endif

    Mqtt_SendMessage("limits", MessageBuffer, strlen(MessageBuffer));
}