#include "lcd_lvgl.h"

static lv_disp_buf_t disp_buf;
static lv_color_t buf_1[320 * 20];
static lv_color_t buf_2[320 * 20];
static uint8_t lcd_buf_temp[320*20*2];
lv_disp_t * disp;
lv_disp_drv_t disp_drv;

void LVGL_ILI9341_Flush_Callback(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
  //COLOR_DEPTH需要设置为16 RGB565
  //int32_t x, y;
  int16_t w , h , size ,i;
  w = area->x2 - area->x1 + 1;
  h = area->y2 - area->y1 + 1;
  size = w*h;
  while(HAL_SPI_STATE_READY != HAL_SPI_GetState(&hspi1))
    vTaskDelay(1/portTICK_RATE_MS);
  //设置刷新区域 然后依次传入颜色
  ILI9341_SetPos(area->x1,area->x2,area->y1,area->y2);
  for(i = 0;i<size;i++)
  {
    lcd_buf_temp[2*i] = color_p->full >> 8;
    lcd_buf_temp[2*i + 1] = color_p->full & 0xFF;
    color_p++;
  }
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, lcd_buf_temp, size*2, 100);
//  HAL_SPI_Transmit_DMA(&hspi1, lcd_buf_temp, size*2);
  //for(y = area->y1; y <= area->y2; y++) {
  //  for(x = area->x1; x <= area->x2; x++) {
  //    ILI9341_write_word(color_p->full);
  //      color_p++;
  //  }
  //}
  
  /* IMPORTANT!!!
  * Inform the graphics library that you are ready with the flushing*/
  lv_disp_flush_ready(disp_drv);
}

void LVGL_ILI9341_Init(uint8_t dir)
{
  ILI9341_Init(dir);
  lv_init();
  lv_disp_buf_init(&disp_buf, buf_1, buf_2, 320*20);
  lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
  disp_drv.buffer = &disp_buf;            /*Set an initialized buffer*/
  disp_drv.flush_cb = LVGL_ILI9341_Flush_Callback;        /*Set a flush callback to draw to the display*/
  disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
}