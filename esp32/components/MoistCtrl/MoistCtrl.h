#ifndef _MOISTCTRL_H_
#define _MOISTCTRL_H_

#include "esp_err.h"
#include "mqtt_client.h"

esp_err_t MoistCtrl_Init(void);
int MoistCtrl_GetMoist(uint32_t Timestamp);

#endif /* _MOISTCTRL_H_ */