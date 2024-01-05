
#ifndef _DHT11_H_
#define _DHT11_H_

#include "esp_err.h"
#include "driver/gpio.h"

typedef enum
{
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
} Dht11_Status;

typedef struct
{
    int status;
    int temperature;
    int humidity;
} Dht11_Reading;

esp_err_t DHT11_init(void);
Dht11_Reading DHT11_read(uint32_t CurrentTimestamp);

#endif