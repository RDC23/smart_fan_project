#ifndef _OLED_H
#define _OLED_H

typedef char H_8;
typedef short int H_16;
typedef int H_32;
typedef long int H_64;
typedef unsigned char H_U8;
typedef unsigned short int H_U16;
typedef unsigned int H_U32;
typedef unsigned long int H_U64;
typedef float H_Float;
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;

// IO
#define OLED_SCLK_Set()  GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN4)
#define OLED_SCLK_Clr()  GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN4)

#define OLED_SDIN_Set()  GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN5)
#define OLED_SDIN_Clr()  GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN5)

#define OLED_W  128	
#define OLED_H  64

#define OLED_CMD  0
#define OLED_DATA 1
#define OLED_MODE 0

// SSD1306 OLED Settings and Command Definitions
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

// Currently no scroll functionality, left for possible future use
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A

// SSD1306 OLED Prototype Definitions
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

// Initialize configuration
void YHGUI_Config(void);

// Basic functions
void YHGUI_Clear(void);
void YHGUI_Refresh_Gram(void);
void YHGUI_DrawPoint(H_U8 x, H_U8 y, H_U8 mode);
void YHGUI_Fill(H_U8 x0, H_U8 y0, H_U8 x1, H_U8 y1, H_U8 mode);
void YHGUI_DispChar(H_U8 x, H_U8 y, H_8 c, H_U8 len, H_U8 mode);
void YHGUI_DispChinese(H_U8 x, H_U8 y, H_8 *ch, H_U8 len, H_U8 mode);
void YHGUI_DispString(H_U8 x, H_U8 y, H_8 *c, H_U8 len, H_U8 mode);
void YHGUI_DispDNumber(H_U8 x, H_U8 y, H_U32 num, H_U8 num_len, H_U8 len, H_U8 mode);
void YHGUI_DispRNumber(H_U8 x, H_U8 y, H_U32 num, H_U8 num_len, H_U8 len, H_U8 mode);
void YHGUI_DispLNumber(H_U8 x, H_U8 y, H_U32 num, H_U8 num_len, H_U8 len, H_U8 mode);
void YHGUI_DispFNumber(H_U8 x, H_U8 y, H_Float num, H_U8 num_len, H_U8 num_flen, H_U8 len, H_U8 mode);
void YHGUI_ShowTCoPhoto(H_U8 x, H_U8 y, const H_U8 *photo, H_U8 mode);

#endif
