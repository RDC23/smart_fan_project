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
#define FAN_PWM_MIN_DUTY 135 // Placeholder 20% duty cycle - need experimental verification

/******************************************
 * API FUNCTION DECLARATIONS
 ******************************************/

/*
 * fan_init - Initializes GPIO P1.6 and configures Timer A0 CCR2 to use PWM. Sharing configuration TA0CTL and TA0CCRO with servo to
 * free up Timer A1 for potential ultrasonic use with SMCLK
 */
void fan_init();

/*
 * fan_torque_boost - Boosts the fan to maximum speed to provide high initial torque.
 *
 * This function boosts the fan to its maximum speed by setting the Timer A0 Capture/Compare Register 2 (TA0CCR2) to the maximum PWM duty cycle
 * specified by the constant FAN_PWM_MAX_DUTY. This allows the fan to start with a high torque, which can be beneficial in certain applications
 * where rapid acceleration or overcoming initial inertia is required.
 */
void fan_torque_boost();


#endif
