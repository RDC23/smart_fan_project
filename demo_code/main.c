// main.c
// Author : Ross Cathcart
// Module: Part of Smart Fan Project
// Last update : 29/02/2024 - added logic to enable power output to be diplayed to the screen of MSP430
// This is the main code file to handle the business level logic and interact with components via 
// the API's deveoped.

// SYSTEM LIBRARIES
#include <msp430fr4133.h>
#include <driverlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// USER LIBRARIES
#include "hal_LCD.h"
#include "servo.h"
#include "fancontroller.h"
#include "ultrasonic.h"
#include "thermistor.h"
#include "pir.h"
#include "oled.h"

// DEFINES
#define MODE_BUTTON_PIN BIT6 // P2.6 (On launchpad)
#define ON_OFF_BUTTON BIT2   // P1.2 (On launchpad)
#define BUTTON_DEBOUNCE() (__delay_cycles(100000))
#define ANGLE_INCREMENT_SWEEP 5 // The number of degrees the servo should 'sweep' per iteration of main loop
#define REFRESH_LIM 5 // Iterations of main code before OLED is updated

// TYPEDEFS
typedef enum
{
    SWEEP = 0,
    TRACK = 1,
    STATIC = 2,
    OFF = 3,
} Fanmode_t;

// GLOBALS FLAGS & STATE VARIABLES
volatile PIR_direction_t activated_direction = MID;
volatile Fanmode_t current_mode = STATIC;
volatile bool has_toggled_mode = false;
volatile int last_measured_distance = 0;
volatile int current_dist = 0;
volatile float current_temperature = 0;
volatile bool show_power = false;
volatile double fan_power = 100;
volatile bool fan_on = true;
volatile bool needs_torque_boost = false;
volatile int to_angle = 0;
volatile int refresh_counter = 0;

// HELPER FUNCTIONS
void init_GPIO()
{
    // Setup button to toggle fan mode
    P2DIR &= ~MODE_BUTTON_PIN; // Should this not be input &= ~MODE_BUTTON_PIN?
    P2REN |= MODE_BUTTON_PIN;
    P2OUT |= MODE_BUTTON_PIN;
    P2IES |= MODE_BUTTON_PIN;
    P2IFG &= ~MODE_BUTTON_PIN;
    P2IE |= MODE_BUTTON_PIN;

    // Setup on/ off button to stop/start fan
    P1DIR &= ~ON_OFF_BUTTON;
    P1REN |= ON_OFF_BUTTON;
    P1OUT |= ON_OFF_BUTTON;
    P1IES |= ON_OFF_BUTTON;
    P1IFG &= ~ON_OFF_BUTTON;
    P1IE |= ON_OFF_BUTTON;

    // Setup the fan efficiency LEDs outputs
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
    switch (current_mode)
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
    OLED12864_Configuration();
    ssd1306_init();       
    ssd1306_clearDisplay();
    
    // Enable global interrupts
    __enable_interrupt();

    while (1)
    {
        // 1) Check on/off button
        if (fan_on)
        {
            // 2) Check if fan needs torque boost (starting from stopped)
            if (needs_torque_boost)
            {
                fan_torque_boost();
                needs_torque_boost = false;
            }

            // 3) Handle mode updates
            displayFanMode(current_mode);
            if (has_toggled_mode)
            {
                cycle_fan_mode();
                displayFanMode(current_mode);
                has_toggled_mode = false;
            }

            // 4) Take temperature reading and handle fan speed & power efficiency updates
            current_temperature = measure_temperature();
            fan_temp_to_speed(current_temperature);
            fan_speed_to_power_LED();

            // 5) Update the OLED display
            
            refresh_counter++;
            if (refresh_counter == REFRESH_LIM)
            {
                refresh_counter = 0;

                fan_power = fan_calculate_power();
                char power_buffer[17];
                int length = snprintf(power_buffer, sizeof(power_buffer), "Power = %d mW", (int)fan_power);
                int i = 0;

                for (i = length; i < sizeof(power_buffer); i++)
                {
                    power_buffer[i] = ' ';
                }
                power_buffer[sizeof(power_buffer)-1] = '\0';  // Ensure null termination
                ssd1306_printText(0, 1, power_buffer);

                // Speed display update
                char speed_buffer[19];
                int speed_buffer_usage = snprintf(speed_buffer, sizeof(speed_buffer), "Speed = %d percent", fan_speed);
                int j = 0;
                for (j = speed_buffer_usage; j < sizeof(speed_buffer); j++)
                {
                   speed_buffer[j] = ' ';
                }
                speed_buffer[sizeof(speed_buffer)-1] = '\0';  // Ensure null termination
                ssd1306_printText(0, 3, speed_buffer);


                // Efficiency display update
                char n_buffer[17];

                if (efficiency == LOW)
                {
                 sprintf(n_buffer, "Efficiency =  LOW");
                }
                else if (efficiency == MED)
                {
                  sprintf(n_buffer, "Efficiency =  MED");
                }
                else
                {
                 sprintf(n_buffer, "Efficiency = HIGH");
                }
                ssd1306_printText(0, 5, n_buffer);

            }
            
            // 6) Handle mode defined servo actions
            switch (current_mode)
            {
            case STATIC:
                if (cur_servo_ang != MID)
                {
                    // Face the front and then do not move
                    servo_cycle_gradual(MID);
                }
                break;

            case TRACK:

                // Fire ultrasonic pulse to provide current distance measurement
                ultrasonic_fire_pulse();
                current_dist = ultrasonic_get_distance();

                // If a 'movement event' is detected, turn the servo to face the PIR
                if (movement_event(current_dist))
                {
                    servo_cycle_gradual(activated_direction);
                    // Update the last distance with the target position in the new servo frame
                    ultrasonic_fire_pulse();
                    last_measured_distance = ultrasonic_get_distance();
                }
                break;

            case SWEEP:

                // Increment/ Decrement servo angle based on current angle
                to_angle = cur_servo_ang;

                if (going_cw)
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

        else // Delegating tasks to functions leads to poor responsiveness in the else clause
        {
            // Perform standby mode actions
            fan_stop();
            needs_torque_boost = true;
            displayFanMode(OFF);
            fan_power = 0;
            fan_speed_to_power_LED();

            // Power display update
            char power_buffer[17];
            int length = snprintf(power_buffer, sizeof(power_buffer), "Power = %d mW", (int)fan_power);
            int i = 0;

            for (i = length; i < sizeof(power_buffer); i++)
            {
                power_buffer[i] = ' ';
            }
            power_buffer[sizeof(power_buffer)-1] = '\0';  // Ensure null termination

            ssd1306_printText(0, 1, power_buffer);

            // Speed display update
            char speed_buffer[19];
            int speed_buffer_usage = snprintf(speed_buffer, sizeof(speed_buffer), "Speed = %d percent", fan_speed);
            int j = 0;
            for (j = speed_buffer_usage; j < sizeof(speed_buffer); j++)
            {
              speed_buffer[j] = ' ';
            }
            speed_buffer[sizeof(speed_buffer)-1] = '\0';  // Ensure null termination
            ssd1306_printText(0, 3, speed_buffer);

            // Efficiency display update
           char n_buffer[17];
           sprintf(n_buffer, "Efficiency = HIGH");
           ssd1306_printText(0, 5, n_buffer);
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
    // On/off toggle button
    case P1IV_P1IFG2:
        BUTTON_DEBOUNCE();
        fan_on = !fan_on;
        P1IFG &= ~ON_OFF_BUTTON;
        break;
    }
}

#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR(void)
{
    switch (__even_in_range(P2IV, P2IV_P2IFG7))
    {
    // Mode button toggle
    case P2IV_P2IFG6:
        BUTTON_DEBOUNCE();
        has_toggled_mode = true;
        P2IFG &= ~MODE_BUTTON_PIN;
        break;
    }
}
