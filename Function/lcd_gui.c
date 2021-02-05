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
  lv_obj_t *label[7];
  lv_style_t label_style;
  lv_obj_t *speed;
  lv_obj_t *dir;
  lv_obj_t *log;
  lv_obj_t *log_label;
  lv_style_t log_bg;
}ui;

float speed = 0,dir = 0;
char btn_str[4][10] = {"Start","Stop","Link","Link&Run"};
char led_str[7][12] = {"LED1","LED2","LED3","LED4","Process","Speed m/s :","Dir (deg) :"};
const char *btns_construct[] = {btn_str[0], "\n",btn_str[1], "\n",btn_str[2], "\n",btn_str[3],""};
const char *win_title = "ZBTCar  v0.9   2021/01/30";
char gui_log_str[640] = "#00ff00 TX2>#";
int gui_log_len = 13;
lv_indev_drv_t indev_drv;
void gui_log_append(const char* txt_in);

void gui_TaskFunction(void *pvParameters)
{
  guival_queue_t guival_queuedata;
  LVGL_ILI9341_Init(0);
  gui_init();
  while(1)
  {
    if(xQueueReceive(guival_queue,&guival_queuedata,1)==pdTRUE)
    {
      guival_queuedata.speed;
      lv_label_set_text_fmt(ui.speed,"%0.2f",guival_queuedata.speed/1000.0);
      lv_label_set_text_fmt(ui.dir,"%0.2f",guival_queuedata.dir/100.0);
    }
    lv_task_handler();
  }
}

static void gui_event_handler(lv_obj_t *obj, lv_event_t event)
{
  uint8_t buzqueuedata = 1;
  uint8_t buf[4];
  const uint32_t *id;
  buf[0] = 0xB2;buf[2] = 0x7A;
  if(obj == ui.btns && event == LV_EVENT_VALUE_CHANGED) 
  {
    id = lv_event_get_data();
    switch(*id)
    {
    case 0:
      buf[1] = 0;
      buf[3] = buf[0]+buf[1]+buf[2];
      HAL_UART_Transmit(&PC_UART,buf,4,200);
      break;
    case 1:
      buf[1] = 1;
      buf[3] = buf[0]+buf[1]+buf[2];
      HAL_UART_Transmit(&PC_UART,buf,4,200);
      break;
    case 2:
      gui_log_append("Hello World ZBT!");
      break;
    case 3:
      gui_log_append("I #ff0000 Love# You Forever!");
      break;
    default:
      buzqueuedata = 50;
      break;
    }
    xQueueSendToBack(buz_queue,&buzqueuedata,0);
  }
}

void gui_log_append(const char* txt_in)
{
  int len_append = strlen(txt_in);
  static const char prefix[] = "#00ff00 TX2>#";
  if(len_append+gui_log_len+14>512)
  {
    gui_log_str[0] = 0;
    gui_log_len = len_append + 13;
  }
  else
  {
    gui_log_len = gui_log_len + len_append + 14;
  }
  gui_log_str[1] = 'f';
  gui_log_str[2] = 'f';
  for(int i = gui_log_len-1;i>=0;i--)
    gui_log_str[i+len_append+14] = gui_log_str[i];
  gui_log_str[0] = 0;
  strcat(gui_log_str,prefix);
  strcat(gui_log_str,txt_in);
  if(gui_log_len != len_append+13)
    gui_log_str[len_append+13] = '\n';
  lv_label_set_static_text(ui.log_label, gui_log_str);
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

void gui_encoder_init(void)
{
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_ENCODER;
  indev_drv.read_cb = gui_encoder_input;
  ui.encoder = lv_indev_drv_register(&indev_drv);
  lv_indev_set_group(ui.encoder,ui.group);
}

void gui_lable_init(void)
{
  int i;
  lv_style_copy(&ui.label_style, &lv_style_pretty);
  ui.label_style.text.font = &lv_font_roboto_12;
  for(i = 0;i<4;i++)
  {
    ui.label[i] = lv_label_create(ui.win, NULL);
    lv_label_set_text(ui.label[i],led_str[i]);
    lv_obj_set_size(ui.label[i], 48, 16);
    
    lv_label_set_style(ui.label[i],LV_LABEL_STYLE_MAIN,&ui.label_style);
    lv_label_set_align(ui.label[i], LV_LABEL_ALIGN_CENTER);
    lv_obj_align(ui.label[i], ui.led[i].obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 2);
  }
  ui.label[4] = lv_label_create(ui.win, NULL);
  lv_label_set_text(ui.label[4],led_str[4]);
  lv_obj_set_size(ui.label[4], 96, 16);
  lv_label_set_style(ui.label[4],LV_LABEL_STYLE_MAIN,&ui.label_style);
  lv_label_set_align(ui.label[4], LV_LABEL_ALIGN_RIGHT);
  lv_obj_align(ui.label[4], ui.win, LV_ALIGN_IN_TOP_LEFT, 5, 85);

  for(i = 5;i<7;i++)
  {
    ui.label[i] = lv_label_create(ui.win, NULL);
    lv_label_set_text(ui.label[i],led_str[i]);
    lv_obj_set_size(ui.label[i], 120, 16);
    
    lv_label_set_style(ui.label[i],LV_LABEL_STYLE_MAIN,&ui.label_style);
    lv_label_set_align(ui.label[i], LV_LABEL_ALIGN_LEFT);
    lv_obj_align(ui.label[i], ui.led[3].obj, LV_ALIGN_OUT_RIGHT_TOP, 15, (i-5)*24);
  }
  ui.speed = lv_label_create(ui.win, NULL);
  lv_label_set_text_fmt(ui.speed,"%0.2f",speed);
  lv_obj_align(ui.speed, ui.label[5], LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  
  ui.dir = lv_label_create(ui.win, NULL);
  lv_label_set_text_fmt(ui.dir,"%0.2f",dir);
  lv_obj_align(ui.dir, ui.label[6], LV_ALIGN_OUT_RIGHT_MID, 5, 0);
}

void gui_bar_creat(void)
{
  ui.bar.obj = lv_bar_create(ui.win, NULL);
  lv_obj_set_size(ui.bar.obj, 120, 16);
  lv_obj_align(ui.bar.obj, ui.label[4], LV_ALIGN_OUT_RIGHT_MID,10,0);
  lv_bar_set_anim_time(ui.bar.obj, 1000);
  lv_bar_set_value(ui.bar.obj, 100, LV_ANIM_ON);
}

void gui_log_creat(void)
{
    lv_style_copy(&ui.log_bg, &lv_style_pretty);
    ui.log_bg.body.main_color = lv_color_make(0x30, 0x30, 0x30);
    ui.log_bg.body.grad_color = lv_color_make(0x30, 0x30, 0x30);
    ui.log_bg.body.border.color = LV_COLOR_WHITE;
    ui.log_bg.text.color = lv_color_make(0xE0, 0xE0, 0xE0);
    ui.log_bg.text.font = &lv_font_roboto_12;

    ui.log = lv_win_create(ui.win, NULL);
    lv_win_set_style(ui.log, LV_WIN_STYLE_BG, &ui.log_bg);
    lv_win_set_title(ui.log, "Terminal Log");
    lv_obj_set_size(ui.log, 215, 124);
    lv_obj_align(ui.log, ui.win, LV_ALIGN_IN_TOP_LEFT,5,105);
    lv_win_set_sb_mode(ui.log, LV_SB_MODE_AUTO);
    lv_win_set_layout(ui.log, LV_LAYOUT_PRETTY);
    ui.log_label = lv_label_create(ui.log, NULL);
    lv_label_set_long_mode(ui.log_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(ui.log_label, lv_win_get_width(ui.log));
    lv_label_set_recolor(ui.log_label,true);
    lv_label_set_static_text(ui.log_label, gui_log_str);
}

void gui_init(void)
{
  gui_window_creat();
  gui_btn_creat();
  gui_led_creat();
  gui_encoder_init();
  gui_lable_init();
  gui_bar_creat();
  gui_log_creat();
}