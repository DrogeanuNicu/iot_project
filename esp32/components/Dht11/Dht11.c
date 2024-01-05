
#include "esp_timer.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Dht11.h"

#define DHT11_DELAY_S_FOR_NEW_READ 3u

static gpio_num_t dht_gpio;
static uint32_t last_read_time = 0;
static Dht11_Reading last_read;

static int _waitOrTimeout(uint16_t microSeconds, int level)
{
    int micros_ticks = 0;
    while (gpio_get_level(dht_gpio) == level)
    {
        if (micros_ticks++ > microSeconds)
            return DHT11_TIMEOUT_ERROR;
        ets_delay_us(1); /* Keep this blocking delay as it is to small for the freertos rate. */
    }
    return micros_ticks;
}

static int _checkCRC(uint8_t data[])
{
    if (data[4] == (data[0] + data[1] + data[2] + data[3]))
        return DHT11_OK;
    else
        return DHT11_CRC_ERROR;
}

static void _sendStartSignal()
{
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40); /* Keep this blocking delay as it is to small for the freertos rate. */
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

static int _checkResponse()
{
    /* Wait for next step ~80us*/
    if (_waitOrTimeout(80, 0) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    /* Wait for next step ~80us*/
    if (_waitOrTimeout(80, 1) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    return DHT11_OK;
}
/*
static Dht11_Reading _timeoutError()
{
    Dht11_Reading timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
}
*/

/* static Dht11_Reading _crcError()
{
    Dht11_Reading crcError = {DHT11_CRC_ERROR, -1, -1};
    return crcError;
}
*/

esp_err_t DHT11_init()
{
    vTaskDelay(pdMS_TO_TICKS(1000)); /* Be sure the senzor is in a stable state */
    dht_gpio = GPIO_NUM_32;
    return ESP_OK;
}

Dht11_Reading DHT11_read(uint32_t CurrentTimestamp)
{
    /* Tried to sense too son since last read (dht11 needs ~2 seconds to make a new read) */
    if (CurrentTimestamp - DHT11_DELAY_S_FOR_NEW_READ < last_read_time)
    {
        return last_read;
    }

    last_read_time = CurrentTimestamp;

    uint8_t data[5] = {0, 0, 0, 0, 0};

    _sendStartSignal();

    if (_checkResponse() == DHT11_TIMEOUT_ERROR)
        return last_read;

    /* Read response */
    for (int i = 0; i < 40; i++)
    {
        /* Initial data */
        if (_waitOrTimeout(50, 0) == DHT11_TIMEOUT_ERROR)
            return last_read;

        if (_waitOrTimeout(70, 1) > 28)
        {
            /* Bit received was a 1 */
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    if (_checkCRC(data) != DHT11_CRC_ERROR)
    {
        last_read.status = DHT11_OK;
        last_read.temperature = data[2];
        last_read.humidity = data[0];
        return last_read;
    }
    else
    {
        return last_read;
    }
}