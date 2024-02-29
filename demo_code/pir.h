// pir.h
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 29/02/2024 - added define statements for PIR pins
// Description: Interface file for PIR sensor, containing function prototypes, defines, and custom typedefs.

#ifndef PIR_H_
#define PIR_H_

#include <msp430fr4133.h>

#define PIR_LEFT BIT5  // P1.5
#define PIR_MID BIT4   // P1.4
#define PIR_RIGHT BIT3 // P1.3

typedef enum {
    LEFT = 160,
    MID = 90,
    RIGHT = 20
} PIR_direction_t;

void pir_init();

#endif /* PIR_H_ */
