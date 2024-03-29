// fancontroller.h
// Author: Ross Cathcart
// Module: Part of Smart Fan Project
// Description: Defines pins on MSP430 for fan connection and constants for parameters.

#ifndef FANCONTROLLER_H_
#define FANCONTROLLER_H_

#include <msp430fr4133.h>

#define FAN_PIN BIT6           // P1.6
#define LOW_POWER_LED BIT0     // P5.0
#define MED_POWER_LED BIT2     // P5.2
#define HIGH_POWER_LED BIT3    // P5.3
#define FAN_PWM_MAX_DUTY 640   // Full speed fan
#define FAN_PWM_MIN_DUTY 135   // Min speed fan (no stalling)
#define FAN_MIN_TEMP 10
#define FAN_MAX_TEMP 35

typedef enum
{
    LOW = 0,
    MED = 1,
    HIGH = 2,
} Efficiency_t;


extern int fan_speed;

extern Efficiency_t efficiency;

void fan_init(); // Initializes GPIO P1.6 and Timer A0 CCR2 for PWM.

void fan_torque_boost(); // Boosts fan to maximum speed for high start-up torque.

void fan_to_speed_percent(int speed); // Sets fan speed based on provided percentage.

void fan_temp_to_speed(double temp); // Sets fan speed based on provided temperature.

void fan_stop(); // Stops the fan by setting TA0CCR2 to 0.

double fan_calculate_power(); // Calculates fan power consumption (between 0 and 3 watts).

void fan_speed_to_power_LED(); // Lights corresponding LED indicator based on current speed.

#endif
