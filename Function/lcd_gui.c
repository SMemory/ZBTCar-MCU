#include "lcd_gui.h"
#include "func_drv.h"

struct
{
  lv_obj_t *win;
  lv_obj_t *btns;
  lv_group_t *group;
  lv_indev_t* encoder;
  gui_obj bar;
  gui_obj led[4];
  gui_obj gauge[2];
}ui;

char btn_str[4][10] = {"Start","Stop","Link","Link&Run-"};
const char *btns_construct[] = {btn_str[0], "\n",btn_str[1], "\n",btn_str[2], "\n",btn_str[3],""};
const char *win_title = "ZBTCar  v0.9   2021/01/30";
lv_indev_drv_t indev_drv;

void gui_TaskFunction(void *pvParameters)
{
  LVGL_ILI9341_Init(0);
  gui_init();
  while(1)
  {
    lv_task_handler();
  }
}

static void gui_event_handler(lv_obj_t *obj, lv_event_t event)
{
  uint8_t buzqueuedata = 1;
  if(obj == ui.btns && event == LV_EVENT_VALUE_CHANGED) 
  {
    xQueueSendToBack(buz_queue,&buzqueuedata,0);
  }
}

bool gui_encoder_input(lv_indev_drv_t* drv, lv_indev_data_t*data)
{
  static uint8_t LA = 1,LB = 1,A,B;
  B = HAL_GPIO_ReadPin(EC11_B_GPIO_Port,EC11_B_Pin);
  A = HAL_GPIO_ReadPin(EC11_A_GPIO_Port,EC11_A_Pin);
  if(LA && LB)
  {
      if((!A) && B)
        data->enc_diff = 1;
      else if(A && (!B))
        data->enc_diff = -1;
  }
  if(A || B)
  {
    LA = A;
    LB = B;
  }
  GPIO_PinState ec_cl = HAL_GPIO_ReadPin(EC11_CL_GPIO_Port,EC11_CL_Pin);
  data->state = !ec_cl;
  return false;
}

void gui_window_creat(void)
{
  ui.win = lv_win_create(lv_scr_act(), NULL);
  lv_win_set_title(ui.win, win_title);
}

void gui_btn_creat(void)
{
  ui.btns = lv_btnm_create(ui.win, NULL);
  lv_btnm_set_map(ui.btns, btns_construct);
  lv_obj_set_event_cb(ui.btns, gui_event_handler);
  lv_obj_set_width(ui.btns,90);
  lv_obj_set_height(ui.btns,140);
  lv_obj_align(ui.btns, ui.win, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5);
  ui.group = lv_group_create();
  lv_group_add_obj(ui.group, ui.btns);
}

void gui_led_creat(void)
{
  const uint8_t led_dft_c[4][3] = {{0x33,0xff,0x33},{0xff,0xff,0x00},
  {0xff,0x33,0x00},{0x33,0x99,0xff}};
  for(int i = 0;i<4;i++)
  {
    lv_style_copy(&ui.led[i].style, &lv_style_pretty_color);
    ui.led[i].style.body.radius = LV_RADIUS_CIRCLE;
    
    ui.led[i].style.body.main_color = LV_COLOR_MAKE(led_dft_c[i][0], led_dft_c[i][1], led_dft_c[i][2]);
    ui.led[i].style.body.border.color = LV_COLOR_MAKE(0x00, 0x00, 0x00);
    ui.led[i].style.body.border.width = 1;
    ui.led[i].style.body.border.opa = LV_OPA_30;
    
    /*Create a LED and switch it OFF*/
    ui.led[i].obj  = lv_led_create(ui.win, NULL);
    lv_obj_set_size(ui.led[i].obj,24,24);
    lv_led_set_style(ui.led[i].obj, LV_LED_STYLE_MAIN, &ui.led[i].style);
    lv_obj_align(ui.led[i].obj, ui.win, LV_ALIGN_IN_TOP_LEFT, 24+i*48 , 36);
    lv_led_on(ui.led[i].obj);
  }
}

void gui_bar_creat(void)
{
  ui.bar.obj = lv_bar_create(ui.win, NULL);
  lv_obj_set_size(ui.bar.obj, 120, 16);
  lv_obj_align(ui.bar.obj, ui.win, LV_ALIGN_IN_TOP_RIGHT,-100,100);
  lv_bar_set_anim_time(ui.bar.obj, 1000);
  lv_bar_set_value(ui.bar.obj, 100, LV_ANIM_ON);
}

void gui_encoder_init(void)
{
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_ENCODER;
  indev_drv.read_cb = gui_encoder_input;
  ui.encoder = lv_indev_drv_register(&indev_drv);
  lv_indev_set_group(ui.encoder,ui.group);
}

void gui_init(void)
{
  gui_window_creat();
  gui_btn_creat();
  gui_led_creat();
  gui_bar_creat();
  gui_encoder_init();
}