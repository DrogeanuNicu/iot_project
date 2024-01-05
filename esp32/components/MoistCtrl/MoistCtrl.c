#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_hal.h"
#include "hal/adc_types.h"

#include "MoistCtrl.h"

#define ADC_UNIT ADC_UNIT_1
#define ADC_CHANNEL ADC_CHANNEL_0
#define ADC_RESOLUTION ADC_BITWIDTH_DEFAULT
#define ADC_ATTENUATION ADC_ATTEN_DB_11

#define MOIST_CTRL_SEC_TO_READ 1u

const static char *TAG = "MoistCtrl";
static uint32_t LastTimestamp = 0;
static int LastMoisture = 0;

adc_oneshot_unit_handle_t AdcHandler;

esp_err_t MoistCtrl_Init(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Init");
#endif

    esp_err_t ReturnStatus = ESP_FAIL;
    adc_oneshot_unit_init_cfg_t InitConfig = {
        .unit_id = ADC_UNIT,
    };
    ReturnStatus = adc_oneshot_new_unit(&InitConfig, &AdcHandler);
    if (ESP_OK != ReturnStatus)
    {
        return ReturnStatus;
    }

    adc_oneshot_chan_cfg_t ChanConfig = {
        .bitwidth = ADC_RESOLUTION,
        .atten = ADC_ATTENUATION};
    ReturnStatus = adc_oneshot_config_channel(AdcHandler, ADC_CHANNEL, &ChanConfig);
    if (ESP_OK != ReturnStatus)
    {
        return ReturnStatus;
    }

    return ESP_OK;
}

int MoistCtrl_GetMoist(uint32_t Timestamp)
{
    esp_err_t ReturnStatus = ESP_FAIL;
    int AdcResult = 0;
    int Moisture = 0;
    if (Timestamp - LastTimestamp > MOIST_CTRL_SEC_TO_READ)
    {
        ReturnStatus = adc_oneshot_read(AdcHandler, ADC_CHANNEL, &AdcResult);
        if (ESP_OK == ReturnStatus)
        {
            Moisture = (100 - ((AdcResult / 4095.00) * 100));
            LastMoisture = Moisture;
            return Moisture;
        }
        else
        {
#ifdef CONFIG_PRINT_DEBUG_LOGS
            ESP_LOGI(TAG, "Reading moisture skipped!");
#endif
            return LastMoisture;
        }
    }
    else
    {
        return LastMoisture;
    }
}
