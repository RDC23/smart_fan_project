#ifndef I2C_H_
#define I2C_H_

#include <msp430fr4133.h>

extern unsigned char *PTxData;                                                       // Pointer to TX data
extern unsigned char TxByteCtr;
/* ====================================================================
 * I2C Prototype Definitions
 * ==================================================================== */
void i2c_init(void);
void i2c_write(unsigned char, unsigned char *, unsigned char);

#endif /* I2C_H_ */
