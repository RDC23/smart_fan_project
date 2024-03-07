
#include "driverlib.h"
#include "oled.h"
#include "stdio.h"

char tab[48];
uint32_t distance1 = 0;
uint32_t distance2 = 0;
uint32_t distance3 = 0;

void main(void)
{
    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Setup pins for I2C
    OLED12864_Configuration();

    // Configure Display
    ssd1306_init();             // Initialize SSD1306 OLED
    ssd1306_clearDisplay();     // Clear OLED display

    char txtBlock[93] = "This is a very large some may say huge message to test the functionality of the screen";
    
    
    ssd1306_printTextBlock(0, 1, txtBlock);
    __delay_cycles(10000000);
    ssd1306_clearDisplay();

    // Print Single Lines of Text at each row

    ssd1306_printText(0, 1, "ROSS");
    ssd1306_printText(0, 2, "RUIHANG");
    ssd1306_printText(0, 3, "FRASER");
    ssd1306_printText(0, 4, "UWAIS");
    
    // ssd1306_printText(0, 5, "Line 5");
    // ssd1306_printText(0, 6, "Line 6");
    // ssd1306_printText(0, 7, "Line 7");
    // ssd1306_clearDisplay();

    // Print these at a column other than zero
    // ssd1306_printText(40, 1, "40");
    // ssd1306_printText(50, 2, "50");
    // ssd1306_printText(60, 3, "60");
    // ssd1306_printText(70, 4, "70");
    // ssd1306_printText(80, 5, "80");
    // ssd1306_printText(90, 6, "90");
    // ssd1306_printText(100, 7, "100");

    // Test 1 stop

    while (1);
}
