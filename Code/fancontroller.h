#ifndef FANCONTROLLER_H_
#define FANCONTROLLER_H_

/******************************************
 * INCLUDES
 ******************************************/
#include <msp430.h>

/******************************************
 * DEFINES
 ******************************************/
#define FAN_PIN BIT3 // P8.3
#define FAN_PWM_PERIOD 639// 20kHz
#define FAN_PWM_MAX_DUTY 639 // Placeholder! Needs experimental verification
#define FAN_PWM_MIN_DUTY 0 // Placeholder! Needs experimental verification

/******************************************
 * API FUNCTION DECLARATIONS
 ******************************************/

/*
 * fan_init - Initializes GPIO P8.3 and configures Timer A1 for PWM
 */
void fan_init();


#endif
