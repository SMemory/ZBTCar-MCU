#ifndef __LCD_GUI_H__
#define __LCD_GUI_H__

#include "lcd_lvgl.h"

typedef struct
{
  lv_obj_t *obj;
  lv_style_t style;
}gui_obj;

typedef struct
{
  int16_t speed;
  int16_t dir;
}guival_queue_t;

void gui_TaskFunction(void *pvParameters);
void gui_init(void);
bool gui_encoder_input(lv_indev_drv_t * drv, lv_indev_data_t*data);
#endif