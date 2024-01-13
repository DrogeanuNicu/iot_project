#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
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

#define MAX_MSG_BUF_LEN 100u
#define NUM_OF_INIT_PAR_TASKS 4u

typedef struct
{
    int TempMin;
    int TempMax;
    int HumMin;
    int HumMax;
    int MoistMin;
    int MoistMax;
} SysCtrl_Limits;

typedef struct
{
    esp_err_t (*InitFunction)(void);
    SemaphoreHandle_t Semaphore;
} InitTaskInfo;

typedef struct
{
    SysCtrl_Data *pData;
    SemaphoreHandle_t Semaphore;
} UpdateTaskInfo;

static const char *TAG = "SysCtrl";
static SysCtrl_Limits Limits = {
    .TempMin = 10,
    .TempMax = 50,
    .HumMin = 20,
    .HumMax = 80,
    .MoistMin = 5,
    .MoistMax = 90,
};

static void SetLimits(esp_mqtt_event_handle_t Event);
static void GetLimits(void);
static esp_err_t SensActsInit(void);
static void GenericInitTask(void *pvParameters);
static void UpdateDataTask(void *pvParameters);

void SysCtrl_Init(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Init");
#endif
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(WifiCtrl_Init());
    ESP_ERROR_CHECK(I2cCtrl_Init());

    SemaphoreHandle_t SharedSemaphore = xSemaphoreCreateCounting(NUM_OF_INIT_PAR_TASKS, NUM_OF_INIT_PAR_TASKS);

    InitTaskInfo Tasks[NUM_OF_INIT_PAR_TASKS] = {
        {
            .InitFunction = LcdCtrl_Init,
            .Semaphore = SharedSemaphore,
        },
        {
            .InitFunction = TimeCtrl_Init,
            .Semaphore = SharedSemaphore,
        },
        {
            .InitFunction = Mqtt_Init,
            .Semaphore = SharedSemaphore,
        },
        {
            .InitFunction = SensActsInit,
            .Semaphore = SharedSemaphore,
        },
    };

    if (SharedSemaphore != NULL)
    {
        for (uint8_t i = 0; i < NUM_OF_INIT_PAR_TASKS; i++)
        {
            xTaskCreate(&GenericInitTask, "Init_Task", configMINIMAL_STACK_SIZE * 2, (void *)&Tasks[i], 1, NULL);
        }

        while (uxSemaphoreGetCount(SharedSemaphore) > 0)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        vSemaphoreDelete(SharedSemaphore);
    }
    else /* Serial initialization */
    {
        for (uint8_t i = 0; i < NUM_OF_INIT_PAR_TASKS; i++)
        {
            ESP_ERROR_CHECK(Tasks[i].InitFunction());
        }
    }
}

void SysCtrl_Main(void)
{
    uint32_t LastTimestamp = 0;
    SysCtrl_Data Data = {
        .Timestamp = 0,
        .Dht11Data = {
            .status = DHT11_OK,
            .temperature = 0,
            .humidity = 0,
        },
        .Moist = 0,
        .Fan = false,
        .Pump = false,
    };

    UpdateTaskInfo UpdataInfo = {
        .pData = &Data,
        .Semaphore = xSemaphoreCreateBinary(),
    };
    xTaskCreate(&UpdateDataTask, "Update_Task", 4096, (void *)&UpdataInfo, 1, NULL);

    while (1)
    {
        Data.Timestamp = TimeCtrl_GetTime();
        Data.Dht11Data = DHT11_read(Data.Timestamp);
        Data.Moist = MoistCtrl_GetMoist(Data.Timestamp);
        Data.Fan = (Data.Dht11Data.temperature > Limits.TempMax) |
                   (Data.Dht11Data.humidity > Limits.HumMax);
        Data.Pump = (Data.Moist < Limits.MoistMin);

        FanCtrl_SetState(Data.Fan);
        PumpCtrl_SetState(Data.Pump);

        if (Data.Timestamp - LastTimestamp >= 1)
        {
            xSemaphoreGive(UpdataInfo.Semaphore);
            LastTimestamp = Data.Timestamp;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
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

static void GetLimits(void)
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

static esp_err_t SensActsInit(void)
{
    esp_err_t ReturnStatus = ESP_FAIL;

    ReturnStatus = FanCtrl_Init();
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    ReturnStatus = PumpCtrl_Init();
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    ReturnStatus = MoistCtrl_Init();
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    ReturnStatus = DHT11_init();
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }

    return ReturnStatus;
}

static void GenericInitTask(void *pvParameters)
{
    InitTaskInfo *Task = (InitTaskInfo *)pvParameters;

    if (Task->InitFunction != NULL)
    {
        ESP_ERROR_CHECK(Task->InitFunction());
    }

    xSemaphoreTake(Task->Semaphore, portMAX_DELAY);
    vTaskDelete(NULL);
}

static void UpdateDataTask(void *pvParameters)
{
    UpdateTaskInfo *TaskData = (UpdateTaskInfo *)pvParameters;
    SysCtrl_Data *pData = TaskData->pData;
    SemaphoreHandle_t pSemaphore = TaskData->Semaphore;

    char MessageBuffer[MAX_MSG_BUF_LEN];
    SysCtrl_Data LastData = *pData;
    SysCtrl_DataPos DataPos = {
        .TempRow = 0,
        .TempCol = 2,
        .HumRow = 0,
        .HumCol = 10,
        .MoistRow = 1,
        .MoistCol = 2,
        .FanRow = 1,
        .FanCol = 10,
        .PumpRow = 1,
        .PumpCol = 14,
    };

    GetLimits();

    snprintf(MessageBuffer, MAX_MSG_BUF_LEN, "T:%3d C H:%3d %% %c",
             pData->Dht11Data.temperature,
             pData->Dht11Data.humidity,
             '\0');
    LcdCtrl_MoveCurs(0, 0);
    LcdCtrl_SendString(MessageBuffer);
    snprintf(MessageBuffer, MAX_MSG_BUF_LEN, "M:%3d %% F:%1d P:%1d %c",
             pData->Moist,
             pData->Fan ? 1 : 0,
             pData->Pump ? 1 : 0,
             '\0');
    LcdCtrl_MoveCurs(1, 0);
    LcdCtrl_SendString(MessageBuffer);

    while (1)
    {
        xSemaphoreTake(pSemaphore, portMAX_DELAY);

        LcdCtrl_PrintNewData(&LastData, pData, &DataPos);

        snprintf(MessageBuffer, MAX_MSG_BUF_LEN, "%10lu,%3d,%3d,%3d,%d,%d%c",
                 pData->Timestamp,
                 pData->Dht11Data.temperature,
                 pData->Dht11Data.humidity,
                 pData->Moist,
                 pData->Fan ? 1 : 0,
                 pData->Pump ? 1 : 0,
                 '\0');
        Mqtt_SendMessage("plant", MessageBuffer, strlen(MessageBuffer));

        LastData = *pData;
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "%.*s", MAX_MSG_BUF_LEN, MessageBuffer);
#endif
    }
}