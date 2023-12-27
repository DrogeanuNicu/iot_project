#ifndef _SYSCTRL_H_
#define _SYSCTRL_H_

#include "mqtt_client.h"

void SysCtrl_Init(void);
void SysCtrl_Main(void);
void SysCtrl_ProcessEvent(esp_mqtt_event_handle_t Event);

#endif /* _SYSCTRL_H_ */