#include "func_drv.h"
xQueueHandle buz_queue;
xQueueHandle guival_queue;
uint8_t rec_fifo[5] = {0};
void drv_TaskFunction(void *pvParameters)
{
  uint8_t buz_data = 0;
  HAL_TIM_PWM_Start(&PWM_TIM,PWM_CH);
  __HAL_TIM_SET_COMPARE(&PWM_TIM,PWM_CH,750);
  HAL_UART_Receive_IT(&PC_UART, rec_fifo, 1);
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