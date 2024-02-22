#ifndef PIR_H_
#define PIR_H_

/******************************************
 * INCLUDES                 
 ******************************************/

#include <msp430fr4133.h>

/******************************************
 * DEFINES                  
 ******************************************/

#define PIR_LEFT BIT3 // P1.5
#define PIR_MID BIT4 // P1.4
#define PIR_RIGHT BIT3 // P1.3

typedef enum {
    LEFT = 160,
    MID = 90,
    RIGHT = 20
} PIR_direction;

/******************************************
 * API FUNCTION DECLARATIONS      
 ******************************************/

void pir_init();

#endif