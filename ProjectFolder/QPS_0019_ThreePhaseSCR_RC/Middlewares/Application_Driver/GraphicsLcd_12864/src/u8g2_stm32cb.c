/*
 * u8g2_stm32cb.c
 *
 *  Created on: Aug 4, 2021
 *      Author: vishweshgm
 */
#include "main.h"
#include "u8g2.h"
#include "display_state.h"
/*extern varaibles*/
extern SPI_HandleTypeDef hspi2;

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
    U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
    U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT: /* Delay and GPIO initialization, I already been initialized */
            break;
        case U8X8_MSG_DELAY_MILLI: /* Ms delay function */
//            HAL_Delay(arg_int);
//            vTaskDelay(arg_int*1000);
//            osDelay(arg_int);
//            delay_ms(arg_int);
            break;
        case U8X8_MSG_GPIO_CS: /* Chip select signal control, but it did not seem to dim */
        	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, (GPIO_PinState)arg_int);
            break;
        case U8X8_MSG_GPIO_DC: /* Set the DC pin, pin control DC transmission data or command */
            break;
        case U8X8_MSG_GPIO_RESET: /* Reset the GPIO */
            break;
    }
    return 1;
}

void u8g2Init(u8g2_t *u8g2)
{
	u8g2_Setup_st7920_s_128x64_2(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);  // Initialize u8g2 structure
	u8g2_InitDisplay(u8g2); // initializes the selected chip, after initialization is completed, the display is turned off
//	u8g2_SetPowerSave(u8g2, 0); // turn on the monitor
}
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_BYTE_SEND: /* Bytes of data transmitted through the SPI arg_int */
           HAL_SPI_Transmit(&hspi2, (uint8_t *) arg_ptr, arg_int, 10000); /* Note that to manage its own chip select signal */
            break;
        case U8X8_MSG_BYTE_INIT: /* Initialize function, here I have not filled out the SPI initialization */
            break;
        case U8X8_MSG_BYTE_SET_DC: /* Set the DC pin, pin control DC transmission data or command */
            break;
        case U8X8_MSG_BYTE_START_TRANSFER: /* Before starting the transfer operation will be performed, if the software using the chip select can be controlled here */
        	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, (GPIO_PinState)GPIO_PIN_SET);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER: /* Operation performed after transmission, the use of the software if the chip select can be controlled here */
        	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, (GPIO_PinState)GPIO_PIN_RESET);
            break;
        default:
            return 0;
    }
    return 1;
}
/* Log draw demo official of */
void draw(u8g2_t *u8g2)
{
    u8g2_SetFontMode(u8g2, 1); /* Font selection mode */
    u8g2_SetFontDirection(u8g2, 0); /* Font selection direction */
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf); /* Font selection */
    u8g2_DrawStr(u8g2, 0, 20, "U");

    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");

    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
}
