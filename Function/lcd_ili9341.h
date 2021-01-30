#ifndef __LCD_ILI9341_H__
#define __LCD_ILI9341_H__

#include "main.h"
#include "cmsis_os.h"
#include "freertos.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
void ILI9341_write_word(uint16_t dat);
void ILI9341_write_databuf(uint8_t* data,uint16_t size);
void ILI9341_write_data(uint8_t d);
void ILI9341_write_command(uint8_t c);
void ILI9341_SetPos(uint16_t x0,uint16_t x1,uint16_t y0,uint16_t y1);
void ILI9341_Init(uint8_t direction);
#endif