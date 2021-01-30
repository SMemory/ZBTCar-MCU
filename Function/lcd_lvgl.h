#ifndef __LCD_LVGL_H__
#define __LCD_LVGL_H__

#include "lcd_ili9341.h"
#include "lvgl.h"

void LVGL_ILI9341_Flush_Callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void LVGL_ILI9341_Init(uint8_t dir);
#endif