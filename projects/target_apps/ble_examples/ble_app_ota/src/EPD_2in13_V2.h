/*****************************************************************************
 * | File      	:   EPD_2in13_V2.h
 * | Author      :   Waveshare team
 * | Function    :   2.13inch e-paper V2
 * | Info        :
 *----------------
 * |	This version:   V3.0
 * | Date        :   2019-06-13
 * | Info        :
 * -----------------------------------------------------------------------------
 * V3.0(2019-06-13):
 * 1. Thay đổi tên hàm:
 *    EPD_Reset() => EPD_2IN13_V2_Reset()
 *    EPD_SendCommand() => EPD_2IN13_V2_SendCommand()
 *    EPD_SendData() => EPD_2IN13_V2_SendData()
 *    EPD_WaitUntilIdle() => EPD_2IN13_V2_ReadBusy()
 *    EPD_Init() => EPD_2IN13_V2_Init()
 *    EPD_Clear() => EPD_2IN13_V2_Clear()
 *    EPD_Display() => EPD_2IN13_V2_Display()
 *    EPD_Sleep() => EPD_2IN13_V2_Sleep()
 * 2. Thêm hàm:
 *    EPD_2IN13_V2_DisplayPartBaseImage()
 * -----------------------------------------------------------------------------
 * V2.0(2018-11-14):
 * 1. Loại bỏ: ImageBuff[EPD_HEIGHT * EPD_WIDTH / 8]
 * 2. Thay đổi: EPD_2IN13_V2_Display(UBYTE *Image)
 *    Cần truyền tham số: con trỏ đến dữ liệu cache
 * 3. Thay đổi hằng số chân:
 *    EPD_RST -> EPD_RST_PIN
 *    EPD_DC -> EPD_DC_PIN
 *    EPD_CS -> EPD_CS_PIN
 *    EPD_BUSY -> EPD_BUSY_PIN
 ******************************************************************************/
#ifndef _EPD_2IN13_V2_H_
#define _EPD_2IN13_V2_H_
#include "gpio.h"
#include "spi.h"
#include "stdbool.h"
#include "stdint.h"
#include "systick.h"
#include "user_periph_setup.h"
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

// Độ phân giải màn hình
#define EPD_2IN13_V2_WIDTH 104
#define EPD_2IN13_V2_HEIGHT 212

#define EPD_2IN13_V2_FULL 0
#define EPD_2IN13_V2_PART 1

/*
EPD_POWER  P23 H

BUSY              P20
RST                 P07
DC                  p05
CS                   P21
SCLK               P00
SDI                  P06

*/

// #define EPD_SCLK_H   GPIO_SetActive(GPIO_PORT_0, GPIO_PIN_1)
// #define EPD_SCLK_L   GPIO_SetInactive(GPIO_PORT_0, GPIO_PIN_1)
// #define EPD_SDI_H    GPIO_SetActive(GPIO_PORT_2, GPIO_PIN_0)
// #define EPD_SDI_L    GPIO_SetInactive(GPIO_PORT_2, GPIO_PIN_0)

#define EPD_SCLK_H GPIO_SetActive(GPIO_PORT_0, GPIO_PIN_0)
#define EPD_SCLK_L GPIO_SetInactive(GPIO_PORT_0, GPIO_PIN_0)
#define EPD_SDI_H GPIO_SetActive(GPIO_PORT_0, GPIO_PIN_6)
#define EPD_SDI_L GPIO_SetInactive(GPIO_PORT_0, GPIO_PIN_6)
#define EPD_DC_H GPIO_SetActive(GPIO_PORT_0, GPIO_PIN_5)
#define EPD_DC_L GPIO_SetInactive(GPIO_PORT_0, GPIO_PIN_5)
#define EPD_CS_H GPIO_SetActive(GPIO_PORT_2, GPIO_PIN_1)
#define EPD_CS_L GPIO_SetInactive(GPIO_PORT_2, GPIO_PIN_1)
#define EPD_RST_H GPIO_SetActive(GPIO_PORT_0, GPIO_PIN_7)
#define EPD_RST_L GPIO_SetInactive(GPIO_PORT_0, GPIO_PIN_7)
#define EPD_BUSY GPIO_GetPinStatus(GPIO_PORT_2, GPIO_PIN_0)

#define DEV_Delay_ms(ms) systick_wait(ms * 1000)

void EPD_GPIO_init(void);
void DEV_SPI_WriteByte(uint8_t sdbyte);
void EPD_2IN13_V2_Init(UBYTE Mode);
void EPD_2IN13_V2_Clear(void);
void EPD_2IN13_V2_Display(UBYTE *Image);
void EPD_2IN13_V2_DisplayPart(UBYTE *Image);
void EPD_2IN13_V2_DisplayPartBaseImage(UBYTE *Image);
void EPD_2IN13_V2_Sleep(void);
void EPD_2IN13_V2_TurnOnDisplayPart(void);
void EPD_2IN13_V2_TurnOnDisplay(void);
void EPD_DrawImage(const uint8_t *image, uint16_t x, uint16_t y, uint16_t w,
                   uint16_t h, bool part);
void EPD_SetWindow(uint16_t x, uint8_t y_x8, uint16_t x_size,
                   uint8_t y_size_x8);
#endif
