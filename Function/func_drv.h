#ifndef __FUNC_DRV_H__
#define __FUNC_DRV_H__

#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "tim.h"
#include "usart.h"
#include "lcd_gui.h"

extern xQueueHandle buz_queue;
extern xQueueHandle guival_queue;

void drv_TaskFunction(void *pvParameters);
#endif