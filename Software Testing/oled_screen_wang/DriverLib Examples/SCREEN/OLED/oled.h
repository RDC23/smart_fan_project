/***********************************************************************************

***********************************************************************************/	
#ifndef _OLED_H
#define _OLED_H
//#include "sys.h"
typedef                   char H_8;
typedef          short     int H_16;
typedef                    int H_32;
typedef          long      int H_64;

typedef unsigned          char H_U8;
typedef unsigned short     int H_U16;
typedef unsigned           int H_U32;
typedef unsigned long      int H_U64;

typedef                  float H_Float;

typedef unsigned          char u8;
typedef unsigned short     int u16;
typedef unsigned           int u32;

//typedef unsigned           char uint8_t;
//typedef unsigned short     int  uint16_t;
//typedef unsigned           int uint32_t;

//#define OLED_SCL_RCC			nop();//RCC_APB2Periph_GPIOB
//#define OLED_SCL_PORT			GPIOB
//#define OLED_SCL_PIN			GPIO_Pin_14


//#define OLED_SDA_RCC			RCC_APB2Periph_GPIOB
//#define OLED_SDA_PORT			GPIOB
//#define OLED_SDA_PIN			GPIO_Pin_15


//IO口
#define OLED_SCLK_Set()  GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN4);//GPIO_SetBits(OLED_SCL_PORT,OLED_SCL_PIN)
#define OLED_SCLK_Clr()  GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN4);//GPIO_ResetBits(OLED_SCL_PORT,OLED_SCL_PIN)

#define OLED_SDIN_Set()  GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN5);//GPIO_SetBits(OLED_SDA_PORT,OLED_SDA_PIN)
#define OLED_SDIN_Clr()  GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN5);//GPIO_ResetBits(OLED_SDA_PORT,OLED_SDA_PIN)

#define OLED_W  128	
#define OLED_H  64

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define OLED_MODE 0



/* ====================================================================
 * SSD1306 OLED Settings and Command Definitions
 * ==================================================================== */

#define SSD1306_I2C_ADDRESS             0x3C

#define SSD1306_LCDWIDTH                128
#define SSD1306_LCDHEIGHT               64
#define SSD1306_128_64

#define SSD1306_SETCONTRAST             0x81
#define SSD1306_DISPLAYALLON_RESUME     0xA4
#define SSD1306_DISPLAYALLON            0xA5
#define SSD1306_NORMALDISPLAY           0xA6
#define SSD1306_INVERTDISPLAY           0xA7
#define SSD1306_DISPLAYOFF              0xAE
#define SSD1306_DISPLAYON               0xAF

#define SSD1306_SETDISPLAYOFFSET        0xD3
#define SSD1306_SETCOMPINS              0xDA

#define SSD1306_SETVCOMDETECT           0xDB

#define SSD1306_SETDISPLAYCLOCKDIV      0xD5
#define SSD1306_SETPRECHARGE            0xD9

#define SSD1306_SETMULTIPLEX            0xA8

#define SSD1306_SETLOWCOLUMN            0x00
#define SSD1306_SETHIGHCOLUMN           0x10

#define SSD1306_SETSTARTLINE            0x40

#define SSD1306_MEMORYMODE              0x20
#define SSD1306_COLUMNADDR              0x21
#define SSD1306_PAGEADDR                0x22

#define SSD1306_COMSCANINC              0xC0
#define SSD1306_COMSCANDEC              0xC8

#define SSD1306_SEGREMAP                0xA0

#define SSD1306_CHARGEPUMP              0x8D

#define SSD1306_EXTERNALVCC             0x1
#define SSD1306_SWITCHCAPVCC            0x2

// currently no scroll functionality, left for possible future use
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A


/* ====================================================================
 * SSD1306 OLED Prototype Definitions
 * ==================================================================== */
void ssd1306_init(void);
void ssd1306_command(unsigned char);
void ssd1306_clearDisplay(void);
void ssd1306_setPosition(unsigned char, unsigned char);
void ssd1306_printText(unsigned char, unsigned char, char *);
void ssd1306_printTextBlock(unsigned char, unsigned char, char *);
void ssd1306_printUI32(unsigned char, unsigned char, unsigned int, unsigned char);

unsigned char digits(unsigned int);
void ultoa(unsigned int, char *);
void reverse(char *);



/*初始化配置*/
void YHGUI_Config(void);


/*基本功能*/

/* 清屏 */
void YHGUI_Clear(void);

/* 更新屏幕 */
void YHGUI_Refresh_Gram(void);

/* 画点 */
void YHGUI_DrawPoint(H_U8 x,H_U8 y,H_U8 mode);

/* 填充矩形 */
void YHGUI_Fill(H_U8 x0,H_U8 y0,H_U8 x1,H_U8 y1,H_U8 mode);
	
/* 显示一个字符 */
void YHGUI_DispChar(H_U8 x,H_U8 y,H_8 c,H_U8 len ,H_U8 mode);

/* 显示一个汉字 */
void YHGUI_DispChinese(H_U8 x,H_U8 y,H_8 *ch,H_U8 len,H_U8 mode);

/* 显示字符串，支持汉字 */
void YHGUI_DispString(H_U8 x,H_U8 y,H_8 *c,H_U8 len,H_U8 mode);

/* 定长数字，无符号整型 右对齐*/
void YHGUI_DispDNumber(H_U8 x,H_U8 y,H_U32 num,H_U8 num_len,H_U8 len,H_U8 mode);

/* 不定长数字，无符号整型 右对齐*/
void YHGUI_DispRNumber(H_U8 x,H_U8 y,H_U32 num,H_U8 num_len,H_U8 len,H_U8 mode);

/* 不定长数字，无符号整型 左对齐*/
void YHGUI_DispLNumber(H_U8 x,H_U8 y,H_U32 num,H_U8 num_len,H_U8 len,H_U8 mode);

/* 不定长数字，无符号浮点型 右对齐*/
void YHGUI_DispFNumber(H_U8 x,H_U8 y,H_Float num,H_U8 num_len,H_U8 num_flen,H_U8 len,H_U8 mode);

/* 显示一张双色照片 */
void YHGUI_ShowTCoPhoto(H_U8 x,H_U8 y,const H_U8 *photo,H_U8 mode);








#endif
