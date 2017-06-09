/*
 * i2c.c
 *
 *	I2C master code for MSP430G2553
 *
 *  Created on: Jun 6, 2017
 *      Author: Kristian Sims
 */

#include <msp430.h>
#include "i2c.h"
#include "pins.h"

#define I2C_DIR_WRITE  1
#define I2C_DIR_READ  0
#define I2C_RATE_PRESCALER  40			// 16 MHz / 40 = 400 kHz
// This value may need to be higher; the clock has a wide tolerance, and can
// be as high as 26 MHz. A safer value would be 65 instead of 40. It seems to
// work for UART, though, so I'll leave it alone.

// Status variables for interrupts
static volatile char* i2c_data = 0;
static volatile struct {
	unsigned dir :1;					// 1 for write, 0 for read
	unsigned reg :1;					// Flag to write register before read
	unsigned err :1;					// Flag to signal that error occurred
	unsigned cnt :12;                    // Byte count
} i2c_status = { 0 };

// Initialize hardward i2c
int i2c_init() {

	UCB0CTL1 |= UCSWRST;					// Reset, just in case
	UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;	// Master, I2C mode, synchronous
	UCB0CTL1 = UCSSEL_3 | UCSWRST;			// SMCLK, stay in reset mode

	UCB0BR0 = I2C_RATE_PRESCALER % 256;		// Bit rate
	UCB0BR1 = I2C_RATE_PRESCALER / 256;

	P1SEL |= I2C_SCL | I2C_SDA;				// Configure pins
	P1SEL2 |= I2C_SCL | I2C_SDA;

	// Output 9 clocks with SDA high to reset any stuck devices
	UCB0I2CSA = 0x007F;					// Fake address
	UCB0CTL1 = UCSSEL_3 | UCTXSTT;		// Read, start condition
	UCB0I2CIE = UCNACKIE | UCSTPIE;		// Enable NACK and stop interrupts
	// TODO: enable interrupts at general level

	// Wait until complete (not great--might hang)
	do
		LPM0;
	while (UCB0STAT & UCBBUSY);

	// Reset USCIB0 between uses
	UCB0CTL1 |= UCSWRST;

	return 0;
}

// Write bytes to a device over i2c
int i2c_write(unsigned address, signed reg, unsigned count, char* buffer) {

	// Error if i2c_write called and module not reset (shouldn't happen)
	if (!UCB0CTL1 & UCSWRST)
		return -1;

	// Set state for interrupt
	i2c_data = data;
	i2c_status.dir = I2C_DIR_WRITE;
	i2c_status.cnt = count;

	// Configure USCIB0
	UCB0I2CSA = address;
	UCB0CTL1 = UCSSEL_3 | UCTR | UCTXSTT;	// Start write
	IE2 |= UCB0TXIE;						// Enable data interrupt
	UCB0I2CIE |= UCNACKIE | UCSTPIE;		// Enable NACK and stop interrupts

	// Wait until the operation is complete
	do
		LPM0;
	while (UCB0STAT & UCBBUSY);

	// Reset USCI_B0 for next operation
	UCB0CTL1 = UCSSEL_3 | UCSWRST;

	// Return error if there was a NACK
	return i2c_status.err ? (i2c_status.err = 0, -1) : 0;
}

// Read bytes from a device with optional register
int i2c_read(unsigned address, signed reg, unsigned count, char* buffer) {

	// Error if i2c_read called and module not in reset state
	if (!UCB0CTL1 & UCSWRST)
		return -1;

	// Set state for interrupt
	i2c_data = buffer;
	i2c_status.dir = I2C_DIR_READ;
	i2c_status.cnt = count;

	UCB0I2CSA = address;

	if (reg >= 0) {
		UCB0CTL1 = UCSSEL_3 | UCTR | UCTXSTT;
		// TODO: Does there need to be a wait here?
		UCB0TXBUF = reg;


	return 0;
}

// Interrupt service routines
inline void USCI_B0_I2C_status_ISR() {
	if (UCB0STAT & UCNACKIFG) {			// If NACK detected
		UCB0CTL1 |= UCTXSTP;			// Send stop condition
		i2c_status.err = 1;				// Set error flag
		// TODO: set count to 0 or disable TX/RX?
	}
}

inline void USCI_B0_I2C_TX_RX_ISR() {
	if (IFG2 & UCB0TXIFG) {					// Begin TX interrupt service.
		if (i2c_status.dir) {				// If normal write operation,
			if (i2c_status.cnt) {			// and if data remains,
				--i2c_status.cnt;			// decrement the byte counter
				UCB0TXBUF = *i2c_data++;	// and send the data.
			} else {						// If no data remains,
				UCB0CTL1 |= UCTXSTP;		// transmit the stop condition, and
				IE2 &= ~UCB0TXIE;			// disable the TX interrupt to end.
			}
		} else {
			if (i2c_status.reg) {			// If register write before read,
				UCB0TXBUF = *i2c_data;		// write the register address, and
				i2c_status.reg = 0;			// clear the flag.
			} else {						// But come back again, clear UCTR
				UCB0CTL1 = UCSSEL_3 | UCTXSTT;	// and send the repeated start.
			}
		}
	} else if (IFG2 & UCB0RXIFG) {			// Begin RX interrupt service.
		*i2c_data++ = UCB0RXBUF;			// Copy in the data
	}
}
