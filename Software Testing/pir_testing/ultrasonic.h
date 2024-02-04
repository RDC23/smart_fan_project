#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

/*****************************************
 * DEFINES                 
 *****************************************/

#define TRIG_PIN BIT5 //P1.5
#define ECHO_PIN BIT6 //P1.6
#define CM_CONVERTION 0.00583090379 // From us to cm
#define MAX_RANGE 400.0 // Max range of HC-SR04 ultrasonic sensor is 400 cm

/*****************************************
 * API FUNCTION DECLARATIONS       
 *****************************************/

/**
 * @brief Fires a short 10 us pulse to trigger the ultrasonic sensor
 *
 * This function sends out a short trigger pulse to initiate distance measurement.
 * It uses the TRIG_PIN for sending the trigger signal. The necessary delays to avoid
 * erroneous measurements are incorporated into this function thus there is no need to add
 * delays into code which uses it.
 */
void ultrasonic_fire_pulse();

/**
 * @brief Configures the GPIO pins and timer for ultrasonic sensor operation
 *
 * This function sets up the necessary pins and timer for the ultrasonic sensor.
 * It configures TRIG_PIN as an output for sending trigger pulses and
 * configures ECHO_PIN along with the timer settings for measuring echo duration.
 */
void ultrasonic_setup_pins();

/**
 * @brief Gets the distance measured by the ultrasonic sensor
 *
 * @return The last valid distance measured by the ultrasonic sensor in centimeters.
 */
unsigned long ultrasonic_get_distance();

#endif  ULTRASONIC_H_

