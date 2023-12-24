#ifndef _I2CLCD_H_
#define _I2CLCD_H_

#include "esp_err.h"

esp_err_t LcdCtrl_Init(void);
esp_err_t LcdCtrl_SendCmd(char cmd);
esp_err_t LcdCtrl_SendData(char data);
esp_err_t LcdCtrl_SendString(char *str);
esp_err_t LcdCtrl_MoveCurs(int row, int col);
esp_err_t LcdCtrl_Clear(void);

#endif /* _I2CLCD_H_ */
