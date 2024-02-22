// SYSTEM LIBRARIES
#include <msp430fr4133.h> 
#include <driverlib.h>
#include <math.h>
#include <stdbool.h>

// USER LIBRARIES
#include "hal_LCD.h"
#include "servo.h"
#include "fancontroller.h"
#include "ultrasonic.h"
#include "thermistor.h"
#include "pir.h"

// DEFINES
#define MODE_BUTTON_PIN BIT1// P1.2 (On launchpad)
#define ON_OFF_BUTTON  BIT6 // P2.6 (On launchpad)
#define BUTTON_DEBOUNCE() (__delay_cycles(90000))
#define ANGLE_INCREMENT_SWEEP 5 // The number of degrees the servo should 'sweep' per iteration of main loop

// TYPEDEFS
typedef enum {
    SWEEP = 0,
    TRACK = 1,
    STATIC = 2
} Fanmode_t;

// GLOBALS FLAGS & STATE VARIABLES
volatile PIR_direction activated_direction = MID;
volatile Fanmode_t current_mode = SWEEP;
volatile int last_measured_distance = 0;
volatile bool fan_on = true;
volatile bool needs_torque_boost = false;
volatile bool has_toggled_mode = false;

// HELPER FUNCTIONS
void init_GPIO()
{
    // Setup button to toggle fan mode
    P1DIR |= MODE_BUTTON_PIN;
    P1REN |= MODE_BUTTON_PIN;
    P1OUT |= MODE_BUTTON_PIN;
    P1IE |= MODE_BUTTON_PIN;
    P1IES |= MODE_BUTTON_PIN;
    P1IFG &= ~MODE_BUTTON_PIN;

    // Setup on/ off button to stop/start fan
    P2DIR |= ON_OFF_BUTTON;
    P2REN |= ON_OFF_BUTTON;
    P2OUT |= ON_OFF_BUTTON;
    P2IE |= ON_OFF_BUTTON;
    P2IES |= ON_OFF_BUTTON;
    P2IFG &= ~ON_OFF_BUTTON;

    // Setup the fan efficiency LEDs
    P5DIR |= (LOW_POWER_LED | MED_POWER_LED | HIGH_POWER_LED);
    P5OUT &= ~(LOW_POWER_LED | MED_POWER_LED | HIGH_POWER_LED);
}

bool movement_event(int cdist)
{
    if ((activated_direction != cur_servo_ang) && (abs(cdist - last_measured_distance) > SIG_DELTA_DISTANCE))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void cycle_fan_mode()
{
    switch(current_mode)
    {
    case STATIC:
        current_mode = SWEEP;
        break;
    case SWEEP:
        current_mode = TRACK;
        break;
    case TRACK:
        current_mode = STATIC;
        break;
    }
}

// MAIN CODE
int main(void)
{
    // General Setup
    WDTCTL = WDTPW | WDTHOLD;
    PMM_unlockLPM5();

    // Setup Peripherals
    init_thermistor();
    pir_init();
	ultrasonic_setup_pins();
    init_GPIO();
    fan_init();
    servo_init();
    servo_to_angle(MID);
    Init_LCD();
    displayFanMode(current_mode);

	// Enable global interrupts
    __enable_interrupt();

	while (1)
	{
	    // 1) Handle mode updates
	    if (has_toggled_mode)
	    {
	        BUTTON_DEBOUNCE();
	        cycle_fan_mode();
	        displayFanMode(current_mode);
	        has_toggled_mode = false;
	    }

	    // 2) Take temperature reading and handle fan speed & power efficiency updates
	    float current_temperature =  measure_temperature();		
		fan_temp_to_speed(current_temperature);
		fan_speed_to_power_LED();	    

        // 3) Update the fan power display
        double fan_power = fan_calculate_power();

        // **** RUIHANG FUNCTION TO DISPLAY POWER ****


	    // 4) Handle mode defined servo actions
	    switch(current_mode)
	    {
	    case STATIC:

			// Face the front and then do not move
	        servo_cycle_gradual(MID);
	        break;

	    case TRACK:

	        // Fire ultrasonic pulse to provide current distance measurement
            ultrasonic_fire_pulse();
            int current_dist = ultrasonic_get_distance();

            // If a 'movement event' is detected, turn the servo to face the PIR
            if(movement_event(current_dist))
            {
                servo_cycle_gradual(activated_direction);
                // Update the last distance with the target position in the new servo frame
                ultrasonic_fire_pulse();
                last_measured_distance = ultrasonic_get_distance();
            }	   
            break;

	    case SWEEP:

			// Increment/ Decrement servo angle based on current angle
			int to_angle = cur_servo_ang;

	        if(going_cw)
	        {
	            to_angle += ANGLE_INCREMENT_SWEEP;
	        }
	        else
	        {
	            to_angle -= ANGLE_INCREMENT_SWEEP;
	        }

	        // Handle direction change at bounds
	        if (to_angle >= SERVO_MAX_ANGLE)
	        {
				to_angle = SERVO_MAX_ANGLE;
	            going_cw = false;
	        }
	        else if (to_angle <= SERVO_MIN_ANGLE)
	        {
				to_angle = SERVO_MIN_ANGLE;
	            going_cw = true;
	        }

	        // Move the servo to the new angle
			servo_cycle_gradual(to_angle);		
			break;
	    }

	}
	return 0;
}


// INTERRUPT SERVICE ROUTINES FOR I/O PORTS

#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    {
    // Left PIR sensor
    case P1IV_P1IFG5:
        activated_direction = LEFT;
        P1IFG &= ~PIR_LEFT;
        break;
    // Middle PIR sensor
    case P1IV_P1IFG4:
        activated_direction = MID;
        P1IFG &= ~PIR_MID;
        break;
    // Right PIR sensor
    case P1IV_P1IFG3:
        activated_direction = RIGHT;
        P1IFG &= ~PIR_RIGHT;
        break;
	// Mode toggle button
	case P1IV_P1IFG2:
		has_toggled_mode = true;
		P1IFG &= ~MODE_BUTTON_PIN;
		break;
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR(void)
{
    switch (__even_in_range(P2IV, P2IV_P2IFG7))
    {
    // On-off button
    case P2IV_P2IFG6:
        fan_on ^= fan_on;
        P2IFG &= ~ON_OFF_BUTTON;
        break;
    }
}




