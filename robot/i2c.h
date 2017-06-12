/*
 * i2c.h
 *
 *  Created on: Jun 6, 2017
 *      Author: Kristian Sims
 */

#ifndef I2C_H_
#define I2C_H_

// Public function prototypes
int i2c_init();
int i2c_write(unsigned address, signed reg, unsigned count, char* buffer);
int i2c_read(unsigned address, signed reg, unsigned count, char* buffer);

// Interrupt service routines
inline void USCI_B0_I2C_STATUS_ISR();
inline void USCI_B0_I2C_TX_RX_ISR();

#endif /* I2C_H_ */
