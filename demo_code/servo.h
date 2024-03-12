// servo.h
// Author: Ross Cathcart
// Module: Smart Fan Project
// Last update: 29/02/2024 - added "going_cw" variable to track servo direction
// Description: Provides function prototypes to interact with MG-946R servo motor for controlling its position.

#ifndef SERVO_H_
#define SERVO_H_

#include <driverlib.h>
#include <msp430fr4133.h>

#define SERVO_PIN BIT7    // Servo connected to P1.7
#define PWM_PERIOD 640    // ACLK must count to 640 to generate a 20ms output signal
#define PWM_MIN_DUTY 16   // Theoretically should be 32 - 1ms, 5% duty cycle
#define PWM_MAX_DUTY 78   // Theoretically should be 64 - 2ms, 10% duty cycle
#define SERVO_MIN_ANGLE 0 // degrees
#define SERVO_MAX_ANGLE 180 // degrees
#define SERVO_DELAY()(__delay_cycles(30000)) // Adjust delay cycles arg for delay time

extern int cur_servo_ang;  // Current servo angle accessible to main code
extern bool going_cw; // Variable to track servo direction

void servo_init(); // Initializes GPIO P1.7 and Timer A0 for PWM

void servo_to_angle(double angle); // Maps desired servo angle to PWM duty cycle

void servo_reset_pos(); // Moves servo to 0 degrees reference position

void servo_cycle_gradual(int new_angle); // Gradually changes servo position to new angle

#endif /* SERVO_H_ */
