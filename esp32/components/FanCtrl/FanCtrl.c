#include "esp_log.h"
#include "driver/gpio.h"

#include "FanCtrl.h"

#define FANCTRL_PIN GPIO_NUM_17
#define FAN_STATE_INDEX 2U

typedef enum
{
    FAN_ON = '1',
    FAN_OFF = '0'
} FanState;

static const char *TAG = "FanCtrl";
static volatile bool IsFanOn = false;

esp_err_t FanCtrl_Init(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Init");
#endif

    if (ESP_OK != gpio_set_direction(FANCTRL_PIN, GPIO_MODE_OUTPUT))
    {
        return ESP_FAIL;
    }
    if (ESP_OK != FanCtrl_Stop())
    {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t FanCtrl_Start(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Fan on!");
#endif

    esp_err_t ReturnStatus = ESP_FAIL;
    ReturnStatus = gpio_set_level(FANCTRL_PIN, 0);
    if (ESP_OK == ReturnStatus)
    {
        IsFanOn = true;
    }
    return ReturnStatus;
}

esp_err_t FanCtrl_Stop(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Fan off!");
#endif

    esp_err_t ReturnStatus = ESP_FAIL;
    ReturnStatus = gpio_set_level(FANCTRL_PIN, 1);
    if (ESP_OK == ReturnStatus)
    {
        IsFanOn = false;
    }
    return ReturnStatus;
}

void FanCtrl_ProcessEvent(esp_mqtt_event_handle_t Event)
{
    if (Event->data_len < FAN_STATE_INDEX + 1)
    {
        ESP_LOGE(TAG, "Invalid command! Fan state not specified!");
    }
    else
    {
        switch (Event->data[FAN_STATE_INDEX])
        {
        case FAN_ON:
            FanCtrl_Start();
            break;

        case FAN_OFF:
            FanCtrl_Stop();
            break;

        default:
            ESP_LOGE(TAG, "Invalid fan state!");
            break;
        }
    }
}

bool FanCtrl_GetState(void)
{
    return IsFanOn;
}

void FanCtrl_SetState(bool State)
{
    if (State != IsFanOn)
    {
        if (true == State)
        {
            FanCtrl_Start();
        }
        else
        {
            FanCtrl_Stop();
        }
    }
}