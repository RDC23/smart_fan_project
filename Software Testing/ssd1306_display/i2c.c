#include "i2c.h"
#include <msp430fr4133.h>
#include <stdint.h>

#define SDA BIT2                                                        // i2c sda P5.2
#define SCL BIT3                                                        // i2c scl P5.3

void i2c_init(void) {
    P5SEL0   |= SCL + SDA;                                              // Assign I2C pins to USCI_B0
    UCB0CTL1 |= UCSWRST;                                                // Enable SW reset
    UCB0CTL0  = UCMST + UCMODE_3 + UCSYNC;                              // I2C Master, synchronous mode
    UCB0CTL1  = UCSSEL_2 + UCSWRST;                                     // Use SMCLK, keep SW reset
    UCB0BR0   = 10;                                                     // fSCL = SMCLK/10 = ~100kHz with SMCLK 1MHz
    UCB0BR1   = 0;
    UCB0CTL1 &= ~UCSWRST;                                               // Clear SW reset, resume operation
    UCB0IE |= UCTXIE;                                                  // Enable TX interrupt

} // end i2c_init

void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr) {
    UCB0I2CSA = slave_address;

    PTxData = DataBuffer;
    TxByteCtr = ByteCtr;

    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;                                         // I2C TX, start condition
    while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
    __bis_SR_register(CPUOFF + GIE);                                    // Enter LPM0 w/ interrupts
                                                                        // Remain in LPM0 until all data is TX'd
}

/* =============================================================================
 * The USCIAB0TX_ISR is structured such that it can be used to transmit any
 * number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
 * points to the next byte to transmit.
 * ============================================================================= */
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void) {
    if (TxByteCtr) {                                                    // Check TX byte counter
        UCB0TXBUF = *PTxData++;                                         // Load TX buffer
        TxByteCtr--;                                                    // Decrement TX byte counter
    } else {
        UCB0CTL1 |= UCTXSTP;                                            // I2C stop condition
        UCB0IE &= ~UCTXIE;                                              // Clear USCI_B0 TX int flag
        __bic_SR_register_on_exit(CPUOFF);                              // Exit LPM0
  }
}
