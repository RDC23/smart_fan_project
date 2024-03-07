// oled.c
// Author: Ruihang Wang
// Module: Smart Fan Project
// Last update: 07/03/2024
// Description: Implementation file for OLED display functions.

#include "oled.h"
#include "gpio.h"
#include "oledfont.h"

// Globals
H_U8 OLED_GRAM[8][128];
unsigned char buffer[17];
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);

// Centering array positions
const unsigned char HcenterUL[] = {                                          
    0,  // 0 digits, not used but included to size array correctly
    61, // 1 digit
    58, // 2 digits
    55, // 3 digits
    49, // 4 digits and 1 separator
    46, // 5 digits and 1 separator
    43, // 6 digits and 1 separator
    37, // 7 digits and 2 separators
    34, // 8 digits and 2 separators
    31, // 9 digits and 2 separators
    25  // 10 digits and 3 separators
};

// Write a character to the data buffer which will be transmitted via I2C
void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr) 
{
    unsigned char i;
    for(i = 0; i < ByteCtr; i++)  
    {      
      Write_IIC_Data(DataBuffer[i]);
    }
}

// Initialise and configure the display settings ( Hex Values extracted from SSD1306 datasheet)
void ssd1306_init(void) {
    ssd1306_command(SSD1306_DISPLAYOFF);                                
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);                        
    ssd1306_command(0x80);                                              
    ssd1306_command(SSD1306_SETMULTIPLEX);                              
    ssd1306_command(SSD1306_LCDHEIGHT - 1);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);                          
    ssd1306_command(0x0);                                               
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);                        
    ssd1306_command(SSD1306_CHARGEPUMP);                                
    ssd1306_command(0x14);                                              
    ssd1306_command(SSD1306_MEMORYMODE);                                
    ssd1306_command(0x00);                                              
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                                
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                               
    ssd1306_command(0xCF);
    ssd1306_command(SSD1306_SETPRECHARGE);                              
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);                             
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);                       
    ssd1306_command(SSD1306_NORMALDISPLAY);                             
    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
    ssd1306_command(SSD1306_DISPLAYON);                                 
}

// Screen wrapper to call the I2C write command
void ssd1306_command(unsigned char command) {
    Write_IIC_Command(command);
} 

// Loop through every row and column and clear the char (clearing the entire display)
void ssd1306_clearDisplay(void) {
    ssd1306_setPosition(0, 0);
    unsigned char i;
    for (i = 64; i > 0; i--) {                                          
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
} 

// Set the print position on the screen 
void ssd1306_setPosition(unsigned char column, unsigned char page) {
    if (column > 128) {
        column = 0;                                                     
    }
    if (page > 8) {
        page = 0;                                                       
    }
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(column);                                            
    ssd1306_command(SSD1306_LCDWIDTH-1);                                
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(page);                                              
    ssd1306_command(7);                                                 
} 

// Print a single line of text to the display
void ssd1306_printText(unsigned char x, unsigned char y, char *ptString) {

    ssd1306_setPosition(x, y);

    while (*ptString != '\0') {
        buffer[0] = 0x40;
        if ((x + 5) >= 127) {                                           
            x = 0;                                                       
            y++;                                                         
            ssd1306_setPosition(x, y);                                   
        }
        unsigned char i;
        for(i = 0; i < 5; i++) {
            buffer[i+1] = font_5x7n[*ptString - ' '][i];
        }
        buffer[6] = 0x0;
        i2c_write(SSD1306_I2C_ADDRESS, buffer, 7);
        ptString++;
        x+=6;
    }
} 

// Prints a text block (multiline support) to the display
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

// Prints an unsigned integer value to an X-Y position on the display
void ssd1306_printUI32(unsigned char x, unsigned char y, unsigned int val, unsigned char Hcenter ) {
    char text[14];
    ultoa(val, text);
    if (Hcenter) {
        ssd1306_printText(HcenterUL[digits(val)], y, text);
    } else {
        ssd1306_printText(x, y, text);
    }
} 

// Calculates the number of digits in an unsigned integer
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
}

// Convert an unsigned integer to a string
void ultoa(unsigned int val, char *string) {
    uint8_t i = 0;
    uint8_t j = 0;
    do {
        if (j == 3) {                                                    
            string[i++] = ',';                                          
            j = 0;                                                       
        }
        string[i++] = val % 10 + '0';                                   
        j++;                                                            
    } while ((val /= 10) > 0);
    string[i++] = '\0';                                                 
    reverse(string);                                                    
}

// Simple string reverse
void reverse(char *s) {
    unsigned char i, j;
    unsigned char c;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

// Setup GPIO pins for I2C support using bit-bashing
void OLED12864_Configuration(void)
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN5 + GPIO_PIN4); 
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5); 
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4); 
}

// Setup SDA/ SCL for a new I2C transmission
void OLED_IIC_Start()
{
    OLED_SCLK_Set();
    OLED_SDIN_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

// Stop I2C transmission
void OLED_IIC_Stop()
{
    OLED_SCLK_Set() ;
    OLED_SDIN_Clr();
    OLED_SDIN_Set();
}

// SCL delay
void OLED_IIC_Wait_Ack()
{
    OLED_SCLK_Set() ;
    OLED_SCLK_Clr();
}

// Initialise I2C transmission
void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    unsigned char m, da;
    da = IIC_Byte;
    OLED_SCLK_Clr();
    for(i = 0; i < 8; i++)		
    {
        m = da;
        m = m & 0x80;
        if(m == 0x80)
        {
            OLED_SDIN_Set();
        } 
        else 
        {
            OLED_SDIN_Clr();
        }
        da = da << 1;
        OLED_SCLK_Set();
        OLED_SCLK_Clr();
    }
}

// Setup command signals to begin I2C transmission
void Write_IIC_Command(unsigned char IIC_Command)
{
    OLED_IIC_Start();
    Write_IIC_Byte(0x78);            
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(0x00);			
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Command); 
    OLED_IIC_Wait_Ack();	
    OLED_IIC_Stop();
}

// Write a bit of i2c data
void Write_IIC_Data(unsigned char IIC_Data)
{
    OLED_IIC_Start();
    Write_IIC_Byte(0x78);			
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(0x40);			
    OLED_IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Data);
    OLED_IIC_Wait_Ack();	
    OLED_IIC_Stop();
}

// Write a byte of i2c data
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
    if(cmd) {
        Write_IIC_Data(dat);
    } else {
        Write_IIC_Command(dat);
    }
}

// Set Display Position using X and Y position
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD); 
} 

// Display ON
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);  
    OLED_WR_Byte(0X14, OLED_CMD);  
    OLED_WR_Byte(0XAF, OLED_CMD);  
}

// Display OFF
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD);  
    OLED_WR_Byte(0X10, OLED_CMD);  
    OLED_WR_Byte(0XAE, OLED_CMD);  
}	

// Clear display
void OLED_Clear(void)  
{  
    uint8_t i, n;		    
    for(i = 0; i < 8; i++)  
    {  
        OLED_WR_Byte(0xb0 + i, OLED_CMD);    
        OLED_WR_Byte(0x00, OLED_CMD);      
        OLED_WR_Byte(0x10, OLED_CMD);      
        for(n = 0; n < 128; n++) {
            OLED_WR_Byte(0, OLED_DATA); 
        }
    }
}

// Display ON
void OLED_On(void)  
{  
    uint8_t i, n;		    
    for(i = 0; i < 8; i++)  
    {  
        OLED_WR_Byte(0xb0 + i, OLED_CMD);    
        OLED_WR_Byte(0x00, OLED_CMD);      
        OLED_WR_Byte(0x10, OLED_CMD);      
        for(n = 0; n < 128; n++) {
            OLED_WR_Byte(1, OLED_DATA); 
        }
    }
}