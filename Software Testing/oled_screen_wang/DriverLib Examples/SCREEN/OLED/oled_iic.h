#ifndef __OLED_IIC_H
#define __OLED_IIC_H

#include <inttypes.h>
#include "oled.h"

#define OLED_ADDRESS 0x78 // Default 0x78

extern unsigned int HZ;

unsigned int GB16_NUM(void);
void IIC_GPIO_Config(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_WaitAck(void);
void Write_IIC_Byte(uint8_t _ucByte);
void Write_IIC_Command(uint8_t IIC_Command);
void OLED_Fill(uint8_t fill_Data);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t mode);
void OLED_ShowCH(uint8_t x, uint8_t y, uint8_t *chs);
void OLED_Init(void);

#endif
