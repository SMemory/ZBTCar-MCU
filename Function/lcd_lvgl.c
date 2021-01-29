#include "lcd_lvgl.h"

const uint8_t ILI9341_init_sequence[] = 
{
  0xCB,5,0x39,0x2C,0x00,0x34,0x02,
  0xCF,3,0x00,0XC1,0X30,
  0xE8,3,0x85,0x00,0x78,
  0xEA,2,0x00,0x00,
  0xED,4,0x64,0x03,0X12,0X81,
  0xF7,1,0x20,
  0xC0,1,0x23,
  0xC1,1,0x10,
  0xC5,2,0x3e,0x28,
  0xC7,1,0x86,
  0xFF,1,
  0x3A,1,0x55,
  0xB1,2,0x00,0x18,
  0xB6,3,0x08,0x82,0x27,
  0xF2,1,0x00,
  0x26,1,0x01,
  0xE0,15,0x0F,0x31,0x2B,0x0C,0x0E,0x08,0x4E,
  0xF1,0x37,0x07,0x10,0x03,0x0E,0x09,0x00,
  0XE1,15,0x00,0x0E,0x14,0x03,0x11,0x07,0x31,
  0xC1,0x48,0x08,0x0F,0x0C,0x31,0x36,0x0F,
  0x11,0,
  0xFF,2,50,
  0x29,0,
  0xFF,2,5,
  0xFF,0
};

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

void ILI9341_write_databuf(uint8_t* data,uint16_t size)
{
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, data, size, 100);
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
{       //ÉèÖÃXºÍYË¢ÐÂ´°¿Ú
  static uint8_t xy0[4];
  static uint8_t xy1[4];
  xy0[0] = x0>>8;
  xy0[1] = x0;
  xy0[2] = x1>>8;
  xy0[3] = x1;
  xy1[0] = y0>>8;
  xy1[1] = y0;
  xy1[2] = y1>>8;
  xy1[3] = y1;
  ILI9341_write_command(0x2A);
  ILI9341_write_databuf(xy0,4);
  ILI9341_write_command(0x2B);
  ILI9341_write_databuf(xy1,4);
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
  uint8_t *p = (uint8_t*)ILI9341_init_sequence;
  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_RESET);
  HAL_Delay(20);
  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_SET);
  HAL_Delay(200);
  while(1)
  {
    if(*p == 0xFF)
    {
      if(*(p+1) == 0)
        break; //return
      else if(*(p+1) == 1)
      {
        ILI9341_write_command(0x36);
        if(direction == 0)
          ILI9341_write_data(0x28);	        //48 68ÊúÆÁ//28 E8 ºáÆÁ
        else if(direction == 1)
          ILI9341_write_data(0x48);
        else if(direction == 2)
          ILI9341_write_data(0x68);
        else if(direction == 3)
          ILI9341_write_data(0xE8);
        p += 2;
      }
      else if(*(p+1) == 2)
      {
        HAL_Delay(*(p+2));
        p += 3;
      }
    }
    else
    {
      ILI9341_write_command(*p);
      if(*(p+1))
        ILI9341_write_databuf((p+2),*(p+1));
      p += (*(p+1)+2);
    }
  }
}
void lcd_run(void)
{
  HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_SET);
  HAL_Delay(10);
  HAL_GPIO_WritePin(BUZ_GPIO_Port,BUZ_Pin,GPIO_PIN_RESET);
  ILI9341_Init(0);
  ILI9341_single_colour(0xFFFF);
  while(1);
}