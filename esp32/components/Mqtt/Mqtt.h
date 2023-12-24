#ifndef _MQTT_H_
#define _MQTT_H_

#include "esp_err.h"

esp_err_t Mqtt_Init(void);
esp_err_t Mqtt_SendMessage(char *Topic, char *Data, uint32_t DataLen);

#endif /* _MQTT_H_ */