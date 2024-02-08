#ifndef FANCONTROLLER_H_
#define FANCONTROLLER_H_

/******************************************
 * INCLUDES
 ******************************************/
#include <msp430.h>

/******************************************
 * DEFINES
 ******************************************/
#define FAN_PIN BIT6 // P1.6
#define FAN_PWM_MAX_DUTY 640 // Full speed fan
#define FAN_PWM_MIN_DUTY 135 // Placeholder 20% duty cycle - need experimental verifiaction

/******************************************
 * API FUNCTION DECLARATIONS
 ******************************************/

/*
 * fan_init - Initializes GPIO P1.6 and configures Timer A0 CCR2 to use PWM. Sharing configuration TA0CTL and TA0CCRO with servo to
 * free up Timer A1 for potential ultrasonic use with SMCLK
 */
void fan_init();

#endif
