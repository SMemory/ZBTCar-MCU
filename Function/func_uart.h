#ifndef __FUNC_UART_H__
#define __FUNC_UART_H__

#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "tim.h"
#include "usart.h"
#include "lcd_gui.h"
typedef struct
{
  uint8_t header;
  uint8_t type;
  uint8_t length;
  uint8_t sum;
  int16_t dir;
  int16_t speed;
}CMDPackageType;
extern xQueueHandle buz_queue;
extern xQueueHandle guival_queue;
void test_TaskFunction(void *pvParameters);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
uint8_t checksum(uint8_t *rec,uint8_t len);
void cmd_handler(uint8_t *rec);
#endif