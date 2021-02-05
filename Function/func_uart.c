#include "func_uart.h"
const int DutyMax = 970;        //Right
const int DutyMin = 515;        //Left
const int DutyMid = 750;

volatile int duty = 750;
extern uint8_t rec_fifo[5];

void test_TaskFunction(void *pvParameters)
{
  while(1)
  {
    vTaskDelay(200/portTICK_RATE_MS);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == PC_UART.Instance)
  {
    uint8_t data = rec_fifo[0];
    static uint8_t step = 0;
    static uint8_t data_left = 0;
    static uint8_t rec[104];
    HAL_UART_Receive_IT(&PC_UART, rec_fifo, 1);
    if((data == 0xB1 && step == 0)
      ||(step==1) || (step==2))
    {
      rec[step] = data;
      step++;
    }
    else if(step == 3)
    {
      rec[step] = data;
      step++;
      data_left = rec[2];
    }
    else if(step && data_left)
    {
      rec[step] = data;
      step++;
      data_left--;
    }
    if(step >= 4 && data_left == 0)
    {
      data = checksum(rec,step>8?8:step);
      if(data)
        cmd_handler(rec);
      step = 0;
    }
  }
}

uint8_t checksum(uint8_t *rec,uint8_t len)
{
  uint8_t sum = 0;
  for(int i = 0;i<len;i++)
    sum += rec[i];
  if(sum)
    return 0;
  else
    return 1;
}

void cmd_handler(uint8_t *rec)
{
  static int16_t speed = 0,dir = 0;
  static int duty = 0;
  CMDPackageType *p = (CMDPackageType *)rec;
  portBASE_TYPE woken = pdFALSE;
  guival_queue_t guival_queuedata;
  switch(rec[1])
  {
  case 0x01:      //cmd
    if(speed!=p->speed || dir!=p->dir)
    {
      speed = p->speed;
      dir = p->dir;
      guival_queuedata.speed = speed;
      guival_queuedata.dir = dir;
      xQueueSendToBackFromISR(guival_queue,&guival_queuedata,&woken);
      duty = (int)(dir*0.0556f);
      if(duty<-220)
        duty = -220;
      else if(duty>220)
        duty = 220;
      __HAL_TIM_SET_COMPARE(&PWM_TIM,PWM_CH,duty+DutyMid);
    }
    break;
  case 0x02:      //led
    
    break;
  case 0x03:      //buz
    
    break;
  case 0x04:      //bar
    
    break;
  case 0x05:      //log
    
    break;
  case 0x06:      //str
    
    break;
  }
  if(woken)
    taskYIELD();
}