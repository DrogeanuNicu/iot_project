#ifndef _TIMECTRL_H_
#define _TIMECTRL_H_

#include "esp_err.h"

esp_err_t TimeCtrl_Init(void);
uint32_t TimeCtrl_GetTime(void);

#endif /* _TIMECTRL_H_ */