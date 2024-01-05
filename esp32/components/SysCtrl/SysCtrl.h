#ifndef _SYSCTRL_H_
#define _SYSCTRL_H_

#include "mqtt_client.h"
#include "Dht11.h"

typedef struct
{
    Dht11_Reading Dht11Data;
    int Moist;
    bool Fan;
    bool Pump;
} SysCtrl_Data;

typedef struct
{
    int TempRow;
    int TempCol;
    int HumRow;
    int HumCol;
    int MoistRow;
    int MoistCol;
    int FanRow;
    int FanCol;
    int PumpRow;
    int PumpCol;
} SysCtrl_DataPos;

void SysCtrl_Init(void);
void SysCtrl_Main(void);
void SysCtrl_ProcessEvent(esp_mqtt_event_handle_t Event);

#endif /* _SYSCTRL_H_ */