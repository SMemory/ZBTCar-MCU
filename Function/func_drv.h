#ifndef __FUNC_DRV_H__
#define __FUNC_DRV_H__

#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern xQueueHandle buz_queue;

void drv_TaskFunction(void *pvParameters);
#endif