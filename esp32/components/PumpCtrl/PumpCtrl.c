#include "esp_log.h"
#include "driver/gpio.h"

#include "PumpCtrl.h"

#define PUMPCTRL_PIN GPIO_NUM_16
#define PUMP_STATE_INDEX 2U

typedef enum
{
    PUMP_ON = '1',
    PUMP_OFF = '0'
} PumpState;

static const char *TAG = "PumpCtrl";
volatile bool IsPumpOn = false;

esp_err_t PumpCtrl_Init(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Init");
#endif

    if (ESP_OK != gpio_set_direction(PUMPCTRL_PIN, GPIO_MODE_OUTPUT))
    {
        return ESP_FAIL;
    }
    if (ESP_OK != PumpCtrl_Stop())
    {
        return ESP_FAIL;
    }

    IsPumpOn = false;
    return ESP_OK;
}

esp_err_t PumpCtrl_Start(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Pump on!");
#endif

    esp_err_t ReturnStatus = ESP_FAIL;
    ReturnStatus = gpio_set_level(PUMPCTRL_PIN, 0);
    if (ESP_OK == ReturnStatus)
    {
        IsPumpOn = true;
    }
    return ReturnStatus;
}

esp_err_t PumpCtrl_Stop(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Pump off!");
#endif

    esp_err_t ReturnStatus = ESP_FAIL;
    ReturnStatus = gpio_set_level(PUMPCTRL_PIN, 1);
    if (ESP_OK == ReturnStatus)
    {
        IsPumpOn = false;
    }
    return ReturnStatus;
}

void PumpCtrl_ProcessEvent(esp_mqtt_event_handle_t Event)
{
    if (Event->data_len < PUMP_STATE_INDEX + 1)
    {
        ESP_LOGE(TAG, "Invalid command! Pump state not specified!");
    }
    else
    {
        switch (Event->data[PUMP_STATE_INDEX])
        {
        case PUMP_ON:
            PumpCtrl_Start();
            break;

        case PUMP_OFF:
            PumpCtrl_Stop();
            break;

        default:
            ESP_LOGE(TAG, "Invalid pump state!");
            break;
        }
    }
}

bool PumpCtrl_GetState(void)
{
    return IsPumpOn;
}
