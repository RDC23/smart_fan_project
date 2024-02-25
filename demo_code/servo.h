#ifndef SERVO_H_
#define SERVO_H_

/******************************************
 * INCLUDES                 
 ******************************************/

#include <driverlib.h> // For __delay_cycles()
#include <msp430fr4133.h>

/******************************************
 * DEFINES                  
 ******************************************/

#define SERVO_PIN BIT7 // Servo connected to P1.7
#define PWM_PERIOD 640 // ACLK (32kHz, divider 1) must count to 640 to generate a 20ms output signal)
#define PWM_MIN_DUTY 16 // Theoretically should be 32 - 1ms, 5% duty cycle. Experimentally 16 for servo.
#define PWM_MAX_DUTY 78 // Theoretically should be 64 - 2ms, 10% duty cycle. Experimentally 86 for servo.
#define SERVO_MIN_ANGLE 0 // degrees
#define SERVO_MAX_ANGLE 180 // degrees
#define SERVO_DELAY()(__delay_cycles(50000))// Edit delay cycles arg to alter delay time

/******************************************
 * GLOBALS
 ******************************************/

// Make the current servo angle and direction accessible to main code
extern int cur_servo_ang;
extern bool going_cw;

/******************************************
 * API FUNCTION DECLARATIONS      
 ******************************************/

/*
 * servo_init - Initializes GPIO P1.7 and configures Timer A0 for PWM
 *
 * Initializes GPIO P1.7 for servo control and sets up Timer A0 for PWM generation.
 * Users can adjust the PWM duty cycle by writing a value between PWM_MIN_DUTY and PWM_MAX_DUTY to TA0CCR1.
 * PWM_MIN_DUTY and PWM_MAX_DUTY are defined constants.
 */
void servo_init();

/*
 * servo_to_angle - Maps the desired servo angle to the PWM duty cycle
 *
 * @param angle - Integer between 0 and 180 representing the desired servo angle
 *               The function maps this angle to the PWM duty cycle.
 *               Accepts values between PWM_MIN_DUTY and PWM_MAX_DUTY.
 *               PWM_MIN_DUTY and PWM_MAX_DUTY are defined constants.
 */
void servo_to_angle(double angle);

/*
 * servo_reset_pos - Moves the servo to the 0 degrees reference position
 *
 * Resets the servo position to the 0 degrees reference position.
 * No parameters are required for this function.
 */
void servo_reset_pos();

/*
 * servo_cycle_gradual - Gradually changes the servo position from the current angle to a new angle
 *
 * Incrementally changes the servo position from the current angle
 * to the specified new angle. This function smoothly transitions the servo position.
 *
 * @param new_angle - Integer representing the desired new angle of the servo (0-180)
 *                    The function cycles gradually from the current angle to this new angle.
 */
void servo_cycle_gradual(int new_angle);

#endif /* SERVO_H_ */
