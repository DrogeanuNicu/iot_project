#ifndef _I2CLCD_H_
#define _I2CLCD_H_

#include "esp_err.h"
#include "SysCtrl.h"

esp_err_t LcdCtrl_Init(void);
esp_err_t LcdCtrl_SendCmd(char cmd);
esp_err_t LcdCtrl_SendData(char data);
esp_err_t LcdCtrl_SendString(char *str);
esp_err_t LcdCtrl_MoveCurs(int row, int col);
esp_err_t LcdCtrl_Clear(void);
void LcdCtrl_PrintNewData(SysCtrl_Data *LastData, SysCtrl_Data *Data, SysCtrl_DataPos *DataPos);

#endif /* _I2CLCD_H_ */
