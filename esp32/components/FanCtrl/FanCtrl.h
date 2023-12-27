#ifndef _FANCTRL_H_
#define _FANCTRL_H_

#include "esp_err.h"
#include "mqtt_client.h"

esp_err_t FanCtrl_Init(void);
esp_err_t FanCtrl_Start(void);
esp_err_t FanCtrl_Stop(void);
bool FanCtrl_GetState(void);
void FanCtrl_SetState(bool State);
void FanCtrl_ProcessEvent(esp_mqtt_event_handle_t Event);

#endif /* _FANCTRL_H_ */