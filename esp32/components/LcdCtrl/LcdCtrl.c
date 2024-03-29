
/** Put this in the src folder **/

#include "LcdCtrl.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "SysCtrl.h"
#include "Dht11.h"

#define SLAVE_ADDRESS_LCD 0x4E >> 1 /* 7bit address */
#define I2C_NUM I2C_NUM_0

/* Commands */
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/* Flags for display entry mode */
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

/* Flags for display/cursor shift */
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

/* Flags for function set */
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

/* Flags for backlight control */
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define LCD_MAX_DATA_LEN 8u
#define LCD_MAX_PRINT_TRIES 3u

static const char *TAG = "LCD";

static esp_err_t PrintDataAt(int Row, int Col, char *StringFormat, int Data, int Tries);

esp_err_t LcdCtrl_SendCmd(char cmd)
{
    esp_err_t ReturnStatus;
    char DataU, DataL;
    uint8_t Data[4];
    DataU = (cmd & 0xf0);
    DataL = ((cmd << 4) & 0xf0);
    Data[0] = DataU | 0x0C; // en=1, rs=0
    Data[1] = DataU | 0x08; // en=0, rs=0
    Data[2] = DataL | 0x0C; // en=1, rs=0
    Data[3] = DataL | 0x08; // en=0, rs=0
    ReturnStatus = i2c_master_write_to_device(I2C_NUM, SLAVE_ADDRESS_LCD, Data, 4, 1000);
    if (ReturnStatus != ESP_OK)
    {
        ESP_LOGE(TAG, "Error in sending command");
    }

    return ReturnStatus;
}

esp_err_t LcdCtrl_SendData(char data)
{
    esp_err_t ReturnStatus;
    char DataU, DataL;
    uint8_t Data[4];
    DataU = (data & 0xf0);
    DataL = ((data << 4) & 0xf0);
    Data[0] = DataU | 0x0D; // en=1, rs=0
    Data[1] = DataU | 0x09; // en=0, rs=0
    Data[2] = DataL | 0x0D; // en=1, rs=0
    Data[3] = DataL | 0x09; // en=0, rs=0
    ReturnStatus = i2c_master_write_to_device(I2C_NUM, SLAVE_ADDRESS_LCD, Data, 4, 1000);
    if (ReturnStatus != ESP_OK)
    {
        ESP_LOGE(TAG, "Error in sending data");
    }

    return ReturnStatus;
}

esp_err_t LcdCtrl_Clear(void)
{
    esp_err_t ReturnStatus;

    ReturnStatus = LcdCtrl_SendCmd(LCD_CLEARDISPLAY);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    vTaskDelay(pdMS_TO_TICKS(5));
    return ESP_OK;
}

esp_err_t LcdCtrl_MoveCurs(int row, int col)
{
    switch (row)
    {
    case 0:
        col |= 0x80;
        break;
    case 1:
        col |= 0xC0;
        break;
    }

    return LcdCtrl_SendCmd(col);
}

esp_err_t LcdCtrl_Init(void)
{
    esp_err_t ReturnStatus;
    vTaskDelay(pdMS_TO_TICKS(50));

    ReturnStatus = LcdCtrl_SendCmd(LCD_NOBACKLIGHT);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    ReturnStatus = LcdCtrl_SendCmd(0x30);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    vTaskDelay(pdMS_TO_TICKS(4500));

    ReturnStatus = LcdCtrl_SendCmd(0x30);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    vTaskDelay(pdMS_TO_TICKS(4500));

    ReturnStatus = LcdCtrl_SendCmd(0x30);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    vTaskDelay(pdMS_TO_TICKS(150));

    ReturnStatus = LcdCtrl_SendCmd(0x20);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }

    ReturnStatus = LcdCtrl_SendCmd(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
/*
    ReturnStatus = LcdCtrl_SendCmd(LCD_CLEARDISPLAY);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
 */
    ReturnStatus = LcdCtrl_SendCmd(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }

    ReturnStatus = LcdCtrl_SendCmd(LCD_RETURNHOME);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }
    vTaskDelay(pdMS_TO_TICKS(2000));

    ReturnStatus = LcdCtrl_SendCmd(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    if (ReturnStatus != ESP_OK)
    {
        return ReturnStatus;
    }

    return ReturnStatus;
}

esp_err_t LcdCtrl_SendString(char *str)
{
    esp_err_t ReturnStatus = ESP_FAIL;
    while (*str)
    {
        ReturnStatus = LcdCtrl_SendData(*str++);
        if (ReturnStatus != ESP_OK)
        {
            return ReturnStatus;
        }
    }
    return ReturnStatus;
}

void LcdCtrl_PrintNewData(SysCtrl_Data *LastData, SysCtrl_Data *Data, SysCtrl_DataPos *DataPos)
{
    if (LastData->Dht11Data.temperature != Data->Dht11Data.temperature)
    {
        PrintDataAt(DataPos->TempRow, DataPos->TempCol, "%3d%c", Data->Dht11Data.temperature, LCD_MAX_PRINT_TRIES);
    }

    if (LastData->Dht11Data.humidity != Data->Dht11Data.humidity)
    {
        PrintDataAt(DataPos->HumRow, DataPos->HumCol, "%3d%c", Data->Dht11Data.humidity, LCD_MAX_PRINT_TRIES);
    }

    if (LastData->Moist != Data->Moist)
    {
        PrintDataAt(DataPos->MoistRow, DataPos->MoistCol, "%3d%c", Data->Moist, LCD_MAX_PRINT_TRIES);
    }

    if (LastData->Fan != Data->Fan)
    {
        PrintDataAt(DataPos->FanRow, DataPos->FanCol, "%1d%c", Data->Fan ? 1 : 0, LCD_MAX_PRINT_TRIES);
    }

    if (LastData->Pump != Data->Pump)
    {
        PrintDataAt(DataPos->PumpRow, DataPos->PumpCol, "%1d%c", Data->Pump ? 1 : 0, LCD_MAX_PRINT_TRIES);
    }
}

static esp_err_t PrintDataAt(int Row, int Col, char *StringFormat, int Data, int Tries)
{
    esp_err_t ReturnStatus = ESP_FAIL;
    char Message[LCD_MAX_DATA_LEN];

    for (int i = 0; i < Tries; i++)
    {
        ReturnStatus = LcdCtrl_MoveCurs(Row, Col);
        if (ESP_OK == ReturnStatus)
        {
            snprintf(Message, LCD_MAX_DATA_LEN, StringFormat, Data, '\0');
            ReturnStatus = LcdCtrl_SendString(Message);
            if (ESP_OK == ReturnStatus)
            {
                break;
            }
        }
    }

    return ReturnStatus;
}
