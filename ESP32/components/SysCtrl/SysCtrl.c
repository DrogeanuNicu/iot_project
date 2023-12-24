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

#define MAX_MSG_BUF_LEN 70

static const char *TAG = "SysCtrl";

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

        snprintf(MessageBuffer, MAX_MSG_BUF_LEN, "{\"time\":%10lu,\"temp\":%3d,\"hum\":%3d,\"moist\":%3d,\"fan\":%d,\"pump\":%d}",
                 Timestamp,
                 Dht11Reading.temperature,
                 Dht11Reading.humidity,
                 MoistReading,
                 FanState ? 1 : 0,
                 PumpState ? 1 : 0);

#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "%.*s", MAX_MSG_BUF_LEN, MessageBuffer);
#endif

        Mqtt_SendMessage("plant", MessageBuffer, MAX_MSG_BUF_LEN);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}