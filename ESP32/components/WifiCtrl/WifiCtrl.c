#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "WifiCtrl.h"

static const char *TAG = "WifiCtrl";

static void WifiEventHandler(void *Args, esp_event_base_t EventBase, int32_t EventId, void *EventData);

esp_err_t WifiCtrl_Init(void)
{
    esp_err_t ReturnStatus = ESP_FAIL;
    wifi_init_config_t WifiInitiation = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t WifiConfiguration = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
        },
    };

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    esp_wifi_init(&WifiInitiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, WifiEventHandler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, WifiEventHandler, NULL);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &WifiConfiguration);
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_STA);
    ReturnStatus = esp_wifi_connect();

#ifdef CONFIG_PRINT_DEBUG_LOGS
    if (ESP_OK == ReturnStatus)
    {
        ESP_LOGI(TAG, "Connected to Wifi!");
    }
    else
    {
        ESP_LOGI(TAG, "Couldn't connect to Wifi!");
    }
#endif

    return ReturnStatus;
}

static void WifiEventHandler(void *Args, esp_event_base_t EventBase, int32_t EventId, void *EventData)
{
    esp_err_t ReturnStatus = ESP_FAIL;

    switch (EventId)
    {
    case WIFI_EVENT_AP_START:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "WIFI_EVENT_AP_START");
#endif
        break;

    case WIFI_EVENT_STA_START:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
#endif
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
#endif
        ReturnStatus = esp_wifi_connect();
        if (ESP_FAIL == ReturnStatus)
        {
            ESP_LOGW(TAG, "ESP32 couldn't reconnect to WiFi!");
        }
        break;

    default:
        break;
    }
}
