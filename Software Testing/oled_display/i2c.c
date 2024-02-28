#include "i2c.h"
#include <msp430fr4133.h>
#include <stdint.h>

#define SDA BIT2                                                        // i2c sda P5.2
#define SCL BIT3                                                        // i2c scl P5.3

unsigned char *PTxData;                                                       // Pointer to TX data
unsigned char TxByteCtr;                                                      // number of bytes to TX

void i2c_init(void) {
    P5SEL0   |= SCL;                                             // Assign I2C pins to USCI_B0
    P5SEL0 |= SDA;
    UCB0CTL1 |= UCSWRST;                                                // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;                              // I2C Master, synchronous mode
    UCB0CTL1 |= UCSSEL_2 + UCSWRST;                                     // Use SMCLK, keep SW reset
    UCB0BR0   = 10;                                                     // fSCL = SMCLK/10 = ~100kHz with SMCLK=1MHz
    UCB0BR1   = 0;                         
    UCB0CTL1 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    UCB0IE |= UCTXIE0;                                                 // Enable TX interrupt

} // end i2c_init

void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr) {
    UCB0I2CSA = slave_address;

    PTxData = DataBuffer;
    TxByteCtr = ByteCtr;

    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;                                         // I2C TX, start condition
    while (UCB0CTLW0 & UCTXSTP);                                        // Ensure stop condition got sent
    __bis_SR_register(CPUOFF + GIE);                                    // Remain in LPM0 until all data is TX'd
}


#pragma vector = USCI_B0_VECTOR
__interrupt void USCIB0_ISR(void){
switch(__even_in_range(UCB0IV,0x1E))
{
        case USCI_I2C_UCTXIFG0:
            if (TxByteCtr)
            {                                         // Check TX byte counter
                    UCB0TXBUF = *PTxData++;           // Load TX buffer
                    TxByteCtr--;                      // Decrement TX byte counter
            }
            else
            {
                    UCB0CTL1 |= UCTXSTP;               // I2C stop condition
                    UCB0IFG &= ~UCTXIFG;
                    __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0

            }
          break;                                      // Vector 26: TXIFG0 break;
        case USCI_I2C_UCBCNTIFG: break;               // Vector 28: BCNTIFG
        case USCI_I2C_UCCLTOIFG: break;               // Vector 30: clock low timeout
        case USCI_I2C_UCBIT9IFG: break;               // Vector 32: 9th bit
        default: break;
  }
}
