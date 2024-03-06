/***********************************************************************************
* Filename:     oled.h
* Description:  Header file for OLED display driver
* Author:       [Author Name]
***********************************************************************************/

#include "oled.h"
#include "font_char.h"
#include "font_chinese.h"
#include "gpio.h"
#include "oledfont.h"

H_U8 OLED_GRAM[8][128];	 

//extern  const unsigned char font_5x7n[96][5];

unsigned char buffer[17];     

void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);

/* ====================================================================
 * Horizontal Centering Number Array
 * ==================================================================== */
const unsigned char HcenterUL[] = {                                           // Horizontally center number with separators on screen
                               0,                                       // 0 digits, not used but included to size array correctly
                               61,                                      // 1 digit
                               58,                                      // 2 digits
                               55,                                      // 3 digits
                               49,                                      // 4 digits and 1 separator
                               46,                                      // 5 digits and 1 separator
                               43,                                      // 6 digits and 1 separator
                               37,                                      // 7 digits and 2 separators
                               34,                                      // 8 digits and 2 separators
                               31,                                      // 9 digits and 2 separators
                               25                                       // 10 digits and 3 separators
};

void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr) 
{
    unsigned char i;
    for(i=0;i<ByteCtr;i++)  //20240229
    {
        //OLED_Set_Pos(0,i);
 
        Write_IIC_Data(DataBuffer[i]);
    }
}

void ssd1306_init(void) {
    // SSD1306 init sequence
    ssd1306_command(SSD1306_DISPLAYOFF);                                // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);                        // 0xD5
    ssd1306_command(0x80);                                              // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                              // 0xA8
    ssd1306_command(SSD1306_LCDHEIGHT - 1);

    ssd1306_command(SSD1306_SETDISPLAYOFFSET);                          // 0xD3
    ssd1306_command(0x0);                                               // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);                        // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                                // 0x8D
    ssd1306_command(0x14);                                              // generate high voltage from 3.3v line internally
    ssd1306_command(SSD1306_MEMORYMODE);                                // 0x20
    ssd1306_command(0x00);                                              // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);

    ssd1306_command(SSD1306_SETCOMPINS);                                // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                               // 0x81
    ssd1306_command(0xCF);

    ssd1306_command(SSD1306_SETPRECHARGE);                              // 0xd9
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);                             // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);                       // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                             // 0xA6

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_command(SSD1306_DISPLAYON);                                 //--turn on oled panel
} // end ssd1306_init

void ssd1306_command(unsigned char command) {
    Write_IIC_Command(command);
} // end ssd1306_command

void ssd1306_clearDisplay(void) {
    ssd1306_setPosition(0, 0);
    unsigned char i;
    for (i = 64; i > 0; i--) {                                          // count down for loops when possible for ULP
        unsigned char x;
        for(x = 16; x > 0; x--) {
            if (x == 1) {
                buffer[x-1] = 0x40;
            } else {
                buffer[x-1] = 0x0;
            }
        }
        i2c_write(SSD1306_I2C_ADDRESS, buffer, 17);
    }
} // end ssd1306_clearDisplay

void ssd1306_setPosition(unsigned char column, unsigned char page) {
    if (column > 128) {
        column = 0;                                                     // constrain column to upper limit
    }
    if (page > 8) {
        page = 0;                                                       // constrain page to upper limit
    }
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(column);                                            // Column start address (0 = reset)
    ssd1306_command(SSD1306_LCDWIDTH-1);                                // Column end address (127 = reset)
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(page);                                              // Page start address (0 = reset)
    ssd1306_command(7);                                                 // Page end address
} // end ssd1306_setPosition

void ssd1306_printText(unsigned char x, unsigned char y, char *ptString) {
    ssd1306_setPosition(x, y);
    while (*ptString != '\0') {
        buffer[0] = 0x40;
        if ((x + 5) >= 127) {                                           // char will run off screen
            x = 0;                                                      // set column to 0
            y++;                                                        // jump to next page
            ssd1306_setPosition(x, y);                                  // send position change to oled
        }
        unsigned char i;
        for(i = 0; i< 5; i++) {
            buffer[i+1] = font_5x7n[*ptString - ' '][i];
        }
        buffer[6] = 0x0;
        i2c_write(SSD1306_I2C_ADDRESS, buffer, 7);
        ptString++;
        x+=6;
    }
} // end ssd1306_printText

void ssd1306_printTextBlock(unsigned char x, unsigned char y, char *ptString) {
    char word[12];
    unsigned char i;
    unsigned char endX = x;
    while (*ptString != '\0'){
        i = 0;
        while ((*ptString != ' ') && (*ptString != '\0')) {
            word[i] = *ptString;
            ptString++;
            i++;
            endX += 6;
        }
        word[i++] = '\0';
        if (endX >= 127) {
            x = 0;
            y++;
            ssd1306_printText(x, y, word);
            endX = (i * 6);
            x = endX;
        } else {
            ssd1306_printText(x, y, word);
            endX += 6;
            x = endX;
        }
        ptString++;
    }
}

void ssd1306_printUI32(unsigned char x, unsigned char y, unsigned int val, unsigned char Hcenter ) {
    char text[14];
    ultoa(val, text);
    if (Hcenter) {
        ssd1306_printText(HcenterUL[digits(val)], y, text);
    } else {
        ssd1306_printText(x, y, text);
    }
} // end ssd1306_printUI32

unsigned char digits(unsigned int n) {
    if (n < 10) {
        return 1;
    } else if (n < 100) {
        return 2;
    } else if (n < 1000) {
        return 3;
    } else if (n < 10000) {
        return 4;
    } else if (n < 100000) {
        return 5;
    } else if (n < 1000000) {
        return 6;
    } else if (n < 10000000) {
        return 7;
    } else if (n < 100000000) {
        return 8;
    } else if (n < 1000000000) {
        return 9;
    } else {
        return 10;
    }
} // end digits

void ultoa(unsigned int val, char *string) {
    uint8_t i = 0;
    uint8_t j = 0;
    do {
        if (j==3) {                                                     // we have reached a separator position
            string[i++] = ',';                                          // add a separator to the number string
            j=0;                                                        // reset separator indexer thingy
        }
        string[i++] = val%10 + '0';                                     // add the ith digit to the number string
        j++;                                                            // increment counter to keep track of separator placement
    } while ((val/=10) > 0);
    string[i++] = '\0';                                                 // add termination to string
    reverse(string);                                                    // string was built in reverse, fix that
} // end ultoa

void reverse(char *s) {
    unsigned char i, j;
    unsigned char c;
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
} // end reverse

/* OLED Initialization */
void OLED12864_Configuration(void)
{
  GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN6 + GPIO_PIN7); //Set as output
  GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN6); //Set RST to low
  GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN7);//Set CS to low
}

/**********************************************
* I2C Start
**********************************************/
void OLED_IIC_Start()
{
    OLED_SCLK_Set();
    OLED_SDIN_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

/**********************************************
* I2C Stop
**********************************************/
void OLED_IIC_Stop()
{
    OLED_SCLK_Set() ;
    //	OLED_SCLK_Clr();
    OLED_SDIN_Clr();
    OLED_SDIN_Set();
}

void OLED_IIC_Wait_Ack()
{
    OLED_SCLK_Set() ;
    OLED_SCLK_Clr();
}

/**********************************************
* I2C Write Byte
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    unsigned char m,da;
    da=IIC_Byte;
    OLED_SCLK_Clr();
    for(i=0;i<8;i++)		
    {
        m=da;
        //	OLED_SCLK_Clr();
        m=m&0x80;
        if(m==0x80)
        {OLED_SDIN_Set();}
        else OLED_SDIN_Clr();
        da=da<<1;
        OLED_SCLK_Set();
        OLED_SCLK_Clr();
    }
}

/**********************************************
* I2C Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    OLED_IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(0x00);			//Write command
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Command); 
    OLED_IIC_Wait_Ack();	
    OLED_IIC_Stop();
}

/**********************************************
* I2C Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    OLED_IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(0x40);			//Write data
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Data);
    OLED_IIC_Wait_Ack();	
    OLED_IIC_Stop();
}

void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
    if(cmd) {
        Write_IIC_Data(dat);
    } else {
        Write_IIC_Command(dat);
    }
}

// Set coordinates
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f),OLED_CMD); 
} 

// Display ON
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC command
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

// Display OFF
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC command
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

// Clear display
void OLED_Clear(void)  
{  
    uint8_t i,n;		    
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //Set page address (0~7)
        OLED_WR_Byte (0x00,OLED_CMD);      //Set low column address
        OLED_WR_Byte (0x10,OLED_CMD);      //Set high column address   
        for(n=0;n<128;n++) {
            OLED_WR_Byte(0,OLED_DATA); 
        }
    }
}

void OLED_On(void)  
{  
    uint8_t i,n;		    
    for(i=0;i<8;i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //Set page address (0~7)
        OLED_WR_Byte (0x00,OLED_CMD);      //Set low column address
        OLED_WR_Byte (0x10,OLED_CMD);      //Set high column address   
        for(n=0;n<128;n++) {
            OLED_WR_Byte(1,OLED_DATA); 
        }
    }
}

/* OLED Initialization */
void YHGUI_Config(void)
{
    OLED12864_Configuration();
    OLED_WR_Byte(0xAE,OLED_CMD);//--display off
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
    OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
    OLED_WR_Byte(0x81,OLED_CMD); // contract control
    OLED_WR_Byte(0xFF,OLED_CMD);//--128   
    OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
    OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
    OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
    OLED_WR_Byte(0x00,OLED_CMD);//
    OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
    OLED_WR_Byte(0x80,OLED_CMD);//
    OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
    OLED_WR_Byte(0x05,OLED_CMD);//
    OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
    OLED_WR_Byte(0xF1,OLED_CMD);//
    OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
    OLED_WR_Byte(0x12,OLED_CMD);//
    OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
    OLED_WR_Byte(0x30,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
    OLED_WR_Byte(0x14,OLED_CMD);//
    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
    YHGUI_Clear();
    YHGUI_Refresh_Gram();
}

void YHGUI_Clear(void)
{
    H_U8 i,j;
    for(i=0;i<OLED_H/8;i++) {
        for(j=0;j<OLED_W;j++) {
            OLED_GRAM[i][j] = 0;
        }
    }
}

/* Draw a point */
void YHGUI_DrawPoint(H_U8 x,H_U8 y,H_U8 mode)
{
    H_U8 pos,bx,temp=0;
    if(x>=OLED_W||y>=OLED_H) return; // Out of bounds.
    pos=y/8;
    bx=y%8;
    temp=1<<bx;
    if(mode) {
        OLED_GRAM[pos][x]|= temp;
    } else {
        OLED_GRAM[pos][x]&=~temp;	  
    }
}

void YHGUI_Refresh_Gram(void)
{
    H_U8 i,j;	
    for(i=0;i<OLED_H/8;i++)  
    {
        OLED_Set_Pos(0,i);
        for(j=0;j<OLED_W;j++) {
            Write_IIC_Data(OLED_GRAM[i][j]);
        }
    }
}

void YHGUI_Fill(H_U8 x0,H_U8 y0,H_U8 x1,H_U8 y1,H_U8 mode)
{
    H_U8 i,j;	
    if (x1 < x0 ) {
        i = x1;
        x1 = x0;
        x0 = i;
    }
    if ( y1 < y0 ) {
        i = y1;
        y1 = y0;
        y0 = i;
    }
    for(j=y0;j<=y1;j++) {
        for(i=x0;i<=x1;i++) {
            YHGUI_DrawPoint(i,j,mode);
        }
    }
    YHGUI_Refresh_Gram();
}

H_U32 guimypow(H_U8 m,H_U8 n)
{
    H_U32 result=1;
    while(n--) {
        result*=m;
    }
    return result;
}
