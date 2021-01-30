#include "func_drv.h"
xQueueHandle buz_queue;
void drv_TaskFunction(void *pvParameters)
{
  uint8_t buz_data = 0;
  HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_SET);
  vTaskDelay(20/portTICK_RATE_MS);
  HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_RESET);
  while(1)
  {
    xQueueReceive(buz_queue,&buz_data,portMAX_DELAY);
    HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_SET);
    vTaskDelay(buz_data*10/portTICK_RATE_MS);
    HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_RESET);
    vTaskDelay(500/portTICK_RATE_MS);
  }
  vTaskDelete(NULL);
}