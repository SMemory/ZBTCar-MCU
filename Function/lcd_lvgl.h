#ifndef __LCD_LVGL_H__
#define __LCD_LVGL_H__

#include "main.h"
#include "cmsis_os.h"
#include "freertos.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

void lcd_run(void);

#endif