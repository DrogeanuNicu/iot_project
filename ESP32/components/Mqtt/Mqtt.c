#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Mqtt.h"
#include "FanCtrl.h"
#include "PumpCtrl.h"

#define COMPONENT_INDEX 0U

typedef enum
{
    FAN = '0',
    PUMP = '1'
} COMPONENT;

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t MqttClient;

static void MqttEventHandler(void *Args, esp_event_base_t Base, int32_t EventId, void *EventData);
static void PrintInvalidMqttCommand(void);

esp_err_t Mqtt_Init(void)
{
    esp_err_t ReturnStatus = ESP_FAIL;
    esp_mqtt_client_config_t MqttCfg = {
        .broker = {
            .address = {
                .hostname = CONFIG_ESP_SERVER_ADDRESS,
                .port = CONFIG_ESP_SERVER_PORT,
                .transport = MQTT_TRANSPORT_OVER_TCP,
            },
        },
    };

    MqttClient = esp_mqtt_client_init(&MqttCfg);
    esp_mqtt_client_register_event(MqttClient, ESP_EVENT_ANY_ID, MqttEventHandler, MqttClient);
    ReturnStatus = esp_mqtt_client_start(MqttClient);

#ifdef CONFIG_PRINT_DEBUG_LOGS
    if (ESP_OK == ReturnStatus)
    {
        ESP_LOGI(TAG, "Connected to the Mqtt broker!");
    }
    else
    {
        ESP_LOGI(TAG, "Couldn't connect to the Mqtt broker!");
    }
#endif

    return ReturnStatus;
}

esp_err_t Mqtt_SendMessage(char *Topic, char *Data, uint32_t DataLen)
{
    int ret = esp_mqtt_client_publish(MqttClient, Topic, Data, DataLen, 0, 1);
    return (0 == ret) ? ESP_OK : ESP_FAIL;
}

static void MqttEventHandler(void *Args, esp_event_base_t Base, int32_t EventId, void *EventData)
{
    esp_mqtt_event_handle_t Event = (esp_mqtt_event_handle_t)EventData;
    // esp_mqtt_client_handle_t Client = Event->client;

    switch ((esp_mqtt_event_id_t)EventId)
    {
    case MQTT_EVENT_CONNECTED:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
#endif
        int ret = esp_mqtt_client_subscribe(MqttClient, "server", 0);
        ESP_LOGI(TAG, "subscribed  %d", ret);
        break;

    case MQTT_EVENT_DISCONNECTED:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
#endif
        break;

    case MQTT_EVENT_SUBSCRIBED:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", Event->msg_id);
#endif
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", Event->msg_id);
#endif
        break;

    case MQTT_EVENT_PUBLISHED:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", Event->msg_id);
#endif
        break;

    case MQTT_EVENT_DATA:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", Event->topic_len, Event->topic);
        printf("DATA=%.*s\r\n", Event->data_len, Event->data);
#endif
        if (0 == Event->data_len)
        {
            PrintInvalidMqttCommand();
            break;
        }

        switch (Event->data[COMPONENT_INDEX])
        {
        case FAN:
            FanCtrl_ProcessEvent(Event);
            break;

        case PUMP:
            PumpCtrl_ProcessEvent(Event);
            break;

        default:
            PrintInvalidMqttCommand();
            break;
        }

        break;

    case MQTT_EVENT_ERROR:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
#endif
        break;

    default:
#ifdef CONFIG_PRINT_DEBUG_LOGS
        ESP_LOGI(TAG, "Event: %d", Event->event_id);
#endif
        break;
    }
}

static void PrintInvalidMqttCommand(void)
{
    ESP_LOGE(TAG, "MQTT command is invalid!");
}