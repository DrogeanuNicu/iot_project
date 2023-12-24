#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "esp_timer.h"

static const char *TAG = "TimeCtrl";
static esp_timer_handle_t TimerHandler;
static volatile uint32_t Timestamp = 0;

static void TimerCallback(void *);

esp_err_t TimeCtrl_Init(void)
{
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Init");
#endif
    uint32_t Sec = 0;
    uint32_t uSec = 0;
    esp_sntp_config_t Config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");

    ESP_ERROR_CHECK(esp_netif_sntp_init(&Config));
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to update system time within 10s timeout");
        return ESP_FAIL;
    }
    sntp_get_system_time(&Sec, &uSec);
    Timestamp = Sec;
#ifdef CONFIG_PRINT_DEBUG_LOGS
    ESP_LOGI(TAG, "Unix timestamp: %lu", Timestamp);
#endif

    const esp_timer_create_args_t TimerArgs = {
        .callback = &TimerCallback};
    ESP_ERROR_CHECK(esp_timer_create(&TimerArgs, &TimerHandler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(TimerHandler, 1000000));

    return ESP_OK;
}

uint32_t TimeCtrl_GetTime(void)
{
    return Timestamp;
}

static void TimerCallback(void *)
{
    Timestamp += 1;
}