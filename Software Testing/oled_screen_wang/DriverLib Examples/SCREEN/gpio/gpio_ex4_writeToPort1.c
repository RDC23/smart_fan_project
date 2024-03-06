/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
//!   Write a byte to Port 1
//!
//!   Description: Writes a byte(0xFF) to Port 1 and stays in LPM4 mode
//!   ACLK = n/a, MCLK = SMCLK = default DCO
//!
//!                MSP430FR4133
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST              |
//!            |             P1.x|-->HI
//!            |                 |
//!            |                 |
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - GPIO Port peripheral
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - None.
//!
//******************************************************************************
#include "driverlib.h"

#include "oled.h"
#include "stdio.h"   
   
char tab[48]; 
uint32_t distance1 = 0;
uint32_t distance2 = 0;
uint32_t distance3 = 0;

void main(void)
{
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

   // tset 1 start  
    
  GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN5 + GPIO_PIN4); //?????
  GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN5); //  SDA P1.5
  GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN4);//   SCK P1.4
    
    ssd1306_init();                             // Initialize SSD1306 OLED
    ssd1306_clearDisplay();                     // Clear OLED display
    
    char txtBlock[93] = "This is a long multiline text block. The code will automatically add extra lines as needed.";
    ssd1306_printTextBlock(0, 1, txtBlock);
    //LONG_DELAY;                             // Avoid delay loops in real code, use timer
    ssd1306_clearDisplay();   
        // =========================================
    // Print Single Lines of Text at each row
    // =========================================
    ssd1306_printText(0, 1, "uWAIS DIN 1");
    ssd1306_printText(0, 2, "Line 2");
    ssd1306_printText(0, 3, "Line 3");
    ssd1306_printText(0, 4, "Line 4");
    ssd1306_printText(0, 5, "Line 5");
    ssd1306_printText(0, 6, "Line 6");
    ssd1306_printText(0, 7, "Line 7");

    // Print these at a column other then zero
    ssd1306_printText(40, 1, "40");
    ssd1306_printText(50, 2, "50");
    ssd1306_printText(60, 3, "60");
    ssd1306_printText(70, 4, "70");
    ssd1306_printText(80, 5, "80");
    ssd1306_printText(90, 6, "90");
    ssd1306_printText(100, 7, "100");
    
     // tset 1 stop  
    
     while(1); // 
    
    
    // tset 2 start 
     GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN5 + GPIO_PIN4); //?????
     GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN5); //????
     GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN4);//????
     YHGUI_Config();  //OLED???
	 YHGUI_DispString(0,0," SyS Init...",16,1);  //OLED?????
//    
//        distance1 = 0;
//		distance2 = 0;
//		distance3 = 0;
//		YHGUI_DispString(0,0,"1621 rx test",16,1);  //OLED?????
		//sprintf(tab,"data1:%d",distance1);
		//YHGUI_DispString(0,16,tab,16,1); 
		//sprintf(tab,"data2:%d",distance2);
		//YHGUI_DispString(0,32,tab,16,1);
		//sprintf(tab,"data3:%d",distance3);
		//YHGUI_DispString(0,48,tab,16,1); 
    
  // tset 2 stop 
    
      while(1);
    //Set all P1 pins HI
    //GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN6);
    //GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN7);

    //PMM_unlockLPM5();
    //Enter LPM4 w/interrupt
    //__bis_SR_register(LPM4_bits + GIE);
    //For debugger
    //__no_operation();
}
