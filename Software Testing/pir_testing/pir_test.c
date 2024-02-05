// This program tests the interfacing of the HC-SR501 PIR sensor array with the MSP430 to control servo position.
// When a PIR sensor is activated, it will light the associated LED.

#include <msp430.h> 
#include "driverlib.h"
#include "servo.h"

#define LEFT_PIR_LED BIT4 //P1.4
#define MID_PIR_LED BIT0// DECIDE ON PIN TO ALLOCATE!
#define RIGHT_PIR_LED BIT0// DECIDE ON PIN TO ALLOCATE!

#define PIR_LEFT BIT3 //P1.3
#define PIR_MID BIT0// DECIDE ON PIN TO ALLOCATE!
#define PIR_RIGHT BIT0// DECIDE ON PIN TO ALLOCATE!

// PIR sensors
volatile bool left_pir_activated = false;
volatile bool mid_pir_activated = false;
volatile bool right_pir_activated = false;

// Servo
volatile int ADCValue_ = 0;
volatile int adc_angle = 0;
volatile int cur_ang = 90;

void init_adc()
{
    SYSCFG2 |= ADCPCTL9; //ADC Channel A9 (P8.1) select
    ADCCTL0 |= ADCON; //Switch ADC on and S&H to 4 clock cycles
    ADCCTL1 |= ADCSHP; //Sample and hold pulse mode
    ADCCTL2 |= ADCRES_1; //10 bit conversion resolution
    ADCMCTL0 |= ADCSREF_0 | ADCINCH_9; //Set the reference voltage and channel to A9
    ADCIFG &= ~0x01; //Clear the interrupt flag
    ADCIE |= ADCIE0; //Enable the interrupts
}

// Mapping function for # of delays
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void init_GPIO()
{
    // Setup the LEDs
    P1DIR |= LEFT_PIR_LED;
    P1OUT &= ~LEFT_PIR_LED;

    // Setup the PIR pins
    P1DIR &= ~PIR_LEFT;
    P1REN |= PIR_LEFT;
    P1OUT |= PIR_LEFT;
    P1IE |= PIR_LEFT;
    P1IES &= ~PIR_LEFT; // Low to high transition interrupt for PIR sensor
    P1IFG &= ~PIR_LEFT;
}

// Define the interrupt service routine for port 1
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR(void)
{
    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    {
    // Left PIR sensor
    case P1IV_P1IFG3:
        left_pir_activated = true;
        P1IFG &= ~PIR_LEFT;
        break;
    }
}

// Define the interrupt service routine for the ADC
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
    // Conversion complete
    case ADCIV_ADCIFG:
        ADCValue_ = ADCMEM0; //When a conversion result is written to ADCMEM0, the ADCIFG0 is set.

        // Map ADC value (0-1023) to angle for either servo or fan pwm
        adc_angle = map(ADCValue_, 0, 1023, 0, 180);
        ADC_clearInterrupt(ADC_BASE, ADC_COMPLETED_INTERRUPT_FLAG);

        break;
    }
}


int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PMM_unlockLPM5();
	init_GPIO();
	init_adc();
	servo_init();
	servo_to_angle(90);
	__enable_interrupt();
	
	while(1)
	{
	    ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);
	    servo_cycle_gradual(cur_ang, 90);
	    cur_ang = 90;

	    // Check each interrupt flag and light LED if movement detected

	    if(left_pir_activated)
	    {
	        P1OUT |= LEFT_PIR_LED;
	        servo_cycle_gradual(cur_ang, 180);
	        cur_ang = 180;
	        left_pir_activated = false;
	        __delay_cycles(1000000);
	    }

	    if(mid_pir_activated)
	    {
	        mid_pir_activated = false;
	        P1OUT |= MID_PIR_LED;
	        __delay_cycles(1000000);
	    }

	    if(right_pir_activated)
        {
	        right_pir_activated = false;
            P1OUT |= RIGHT_PIR_LED;
            __delay_cycles(1000000);
        }

	    else
	    {
	        // No activations, clear the LEDs
	        P1OUT &= ~LEFT_PIR_LED;
	        P1OUT &= ~MID_PIR_LED;
	        P1OUT &= ~RIGHT_PIR_LED;
	    }
	}
	return 0;
}
