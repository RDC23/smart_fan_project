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
#define FAN_PWM_MIN_DUTY 135 // Min speed fan (no stalling)
#define FAN_MIN_TEMP 10
#define FAN_MAX_TEMP 30

/******************************************
 * GLOBALS
 ******************************************/
extern int fan_speed;

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

/*
 * fan_to_speed_percent - Sets the fan speed based on the provided speed percentage.
 *
 * Parameters:
 *   speed: The desired speed percentage for the fan, ranging from 0 to 100.
 *
 * This function calculates the corresponding PWM duty cycle based on the provided speed percentage and sets the Timer A0 Capture/Compare Register 2 (TA0CCR2)
 * accordingly. The duty cycle is linearly mapped from the minimum to maximum duty cycle defined by the constants FAN_PWM_MIN_DUTY and FAN_PWM_MAX_DUTY.
 */
void fan_to_speed_percent(int speed);

/*
 * fan_temp_to_speed - Sets the fan speed based on the provided temperature.
 *
 * Parameters:
 *   temp: The temperature value used to determine the fan speed.
 *
 * This function calculates the corresponding fan speed percentage based on the provided temperature and then calls the fan_to_speed_percent function to
 * set the fan speed accordingly. The temperature is assumed to be within the range of 10 to 30 degrees Celsius, where 10 corresponds to 0% fan speed and
 * 30 corresponds to 100% fan speed.
 */
void fan_temp_to_speed(double temp);

/*
 * fan_stop - Stops the fan by directly modifying TA0CCR2 and setting this to 0
 */
void fan_stop();

#endif
