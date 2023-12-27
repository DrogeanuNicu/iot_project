#ifndef _PUMPCTRL_H_
#define _PUMPCTRL_H_

#include "esp_err.h"
#include "mqtt_client.h"

esp_err_t PumpCtrl_Init(void);
esp_err_t PumpCtrl_Start(void);
esp_err_t PumpCtrl_Stop(void);
bool PumpCtrl_GetState(void);
void PumpCtrl_SetState(bool State);
void PumpCtrl_ProcessEvent(esp_mqtt_event_handle_t Event);

#endif /* _PUMPCTRL_H_ */