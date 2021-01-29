#include "lcd_lvgl.h"

void ILI9341_write_command(uint8_t c)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &c, 1, 100);
}

void ILI9341_write_data(uint8_t d)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, &d, 1, 100);
}

void ILI9341_write_word(uint16_t dat)
{
  ILI9341_write_data(dat>>8);
  ILI9341_write_data(dat);
}

void ILI9341_RamAdressSet()
{
  ILI9341_write_command(0x2A);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0xEF);
  ILI9341_write_command(0x2B);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x01);
  ILI9341_write_data(0x3F);
}

void ILI9341_SetPos(uint16_t x0,uint16_t x1,uint16_t y0,uint16_t y1)
{       //设置X和Y刷新窗口
  ILI9341_write_command(0x2A);
  ILI9341_write_data(x0>>8);
  ILI9341_write_data(x0);
  ILI9341_write_data(x1>>8);
  ILI9341_write_data(x1);

  ILI9341_write_command(0x2B);
  ILI9341_write_data(y0>>8);
  ILI9341_write_data(y0);
  ILI9341_write_data(y1>>8);
  ILI9341_write_data(y1);
  ILI9341_write_command(0x2c);
}

void ILI9341_single_colour(uint16_t color)
{
  unsigned int i,j;
  ILI9341_SetPos(0,320-1,0,240-1);
  for (i=0;i<240;i++)
    for (j=0;j<320;j++)
      ILI9341_write_word(color);
}

void ILI9341_Init(uint8_t direction)
{
  //需要先配置端口
  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_RESET);
  HAL_Delay(20);
  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_SET);
  HAL_Delay(200);
  //------------------------------------------------------------------//
  //-------------------Software Reset-------------------------------//
  ILI9341_write_command(0xCB);
  ILI9341_write_data(0x39);
  ILI9341_write_data(0x2C);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x34);
  ILI9341_write_data(0x02);

  ILI9341_write_command(0xCF);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0XC1);
  ILI9341_write_data(0X30);

  ILI9341_write_command(0xE8);
  ILI9341_write_data(0x85);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x78);

  ILI9341_write_command(0xEA);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x00);

  ILI9341_write_command(0xED);
  ILI9341_write_data(0x64);
  ILI9341_write_data(0x03);
  ILI9341_write_data(0X12);
  ILI9341_write_data(0X81);

  ILI9341_write_command(0xF7);
  ILI9341_write_data(0x20);

  ILI9341_write_command(0xC0);          //Power control
  ILI9341_write_data(0x23);             //VRH[5:0]

  ILI9341_write_command(0xC1);          //Power control
  ILI9341_write_data(0x10);             //SAP[2:0];BT[3:0]

  ILI9341_write_command(0xC5);          //VCM control
  ILI9341_write_data(0x3e);             //对比度调节
  ILI9341_write_data(0x28);

  ILI9341_write_command(0xC7);          //VCM control2
  ILI9341_write_data(0x86);             //--

  ILI9341_write_command(0x36);          // Memory Access Control
  if(direction == 0)
    ILI9341_write_data(0x28);	        //48 68竖屏//28 E8 横屏
  else if(direction == 1)
    ILI9341_write_data(0x48);
  else if(direction == 2)
    ILI9341_write_data(0x68);
  else if(direction == 3)
    ILI9341_write_data(0xE8);


  ILI9341_write_command(0x3A);
  ILI9341_write_data(0x55);

  ILI9341_write_command(0xB1);
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x18);

  ILI9341_write_command(0xB6);    // Display Function Control
  ILI9341_write_data(0x08);
  ILI9341_write_data(0x82);
  ILI9341_write_data(0x27);

  ILI9341_write_command(0xF2);    // 3Gamma Function Disable
  ILI9341_write_data(0x00);

  ILI9341_write_command(0x26);    //Gamma curve selected
  ILI9341_write_data(0x01);

  ILI9341_write_command(0xE0);    //Set Gamma
  ILI9341_write_data(0x0F);
  ILI9341_write_data(0x31);
  ILI9341_write_data(0x2B);
  ILI9341_write_data(0x0C);
  ILI9341_write_data(0x0E);
  ILI9341_write_data(0x08);
  ILI9341_write_data(0x4E);
  ILI9341_write_data(0xF1);
  ILI9341_write_data(0x37);
  ILI9341_write_data(0x07);
  ILI9341_write_data(0x10);
  ILI9341_write_data(0x03);
  ILI9341_write_data(0x0E);
  ILI9341_write_data(0x09);
  ILI9341_write_data(0x00);

  ILI9341_write_command(0XE1);    //Set Gamma
  ILI9341_write_data(0x00);
  ILI9341_write_data(0x0E);
  ILI9341_write_data(0x14);
  ILI9341_write_data(0x03);
  ILI9341_write_data(0x11);
  ILI9341_write_data(0x07);
  ILI9341_write_data(0x31);
  ILI9341_write_data(0xC1);
  ILI9341_write_data(0x48);
  ILI9341_write_data(0x08);
  ILI9341_write_data(0x0F);
  ILI9341_write_data(0x0C);
  ILI9341_write_data(0x31);
  ILI9341_write_data(0x36);
  ILI9341_write_data(0x0F);

  ILI9341_write_command(0x11);    //Exit Sleep
  HAL_Delay(50);

  ILI9341_write_command(0x29);    //Display on
  //ILI9341_write_command(0x2c);
  HAL_Delay(5);
}

void lcd_run(void)
{
  HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_SET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_RESET);
  ILI9341_Init(0);
  ILI9341_single_colour(0xFFE0);
  while(1);
}