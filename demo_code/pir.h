// pir.h
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 07/03/2024 - added define statements for PIR pins
// Description: Interface file for PIR sensor, containing function prototypes, defines, and custom typedefs.

#ifndef PIR_H_
#define PIR_H_

#include <msp430fr4133.h>

#define PIR_LEFT BIT4  // P1.4
#define PIR_MID BIT3   // P1.3
#define PIR_RIGHT BIT5 // P1.5

typedef enum {
    LEFT = 20,
    MID = 90,
    RIGHT = 160
} PIR_direction_t;

void pir_init();

#endif /* PIR_H_ */
