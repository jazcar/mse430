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
static volatile unsigned i2c_count = 0;
static enum i2c_state_enum {
	IDLE = 0, WRITE = 2, READ = 4, REG = 6, RESET = 8
};
static volatile enum i2c_state_enum i2c_state = RESET;

int i2c_wait(int timeout) {
	unsigned long timeout_ticks;
	timeout_ticks = timeout > 0 ? ticks + timeout : 0;

	do {
		LPM0;
		if (timeout_ticks && !--timeout_ticks)
			break; //TODO: continue working here
	} while (UCB0STAT & UCBBUSY);
}

// Initialize hardware i2c
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

	// Wait until complete (TODO: hangs sometimes)
	do
		LPM0;
	while (UCB0STAT & UCBBUSY);

	// Reset USCIB0 between uses
	UCB0CTL1 |= UCSWRST;
	i2c_state = IDLE;

	return 0;
}

// Write bytes to a device over i2c
int i2c_write(unsigned address, signed reg, unsigned count, char* buffer) {

	// Error if i2c_write called and module not reset (shouldn't happen)
	if (i2c_state || !UCB0CTL1 & UCSWRST)
		return -1;

	// Set state for interrupt
	i2c_data = buffer;
	i2c_count = count;
	i2c_state = WRITE;

	// Configure USCIB0
	UCB0I2CSA = address;
	UCB0CTL1 = UCSSEL_3 | UCTR | UCTXSTT;	// Start write

	// Send register
	if (reg >= 0)
		UCB0TXBUF = reg;

	// Interrupts
	IE2 |= UCB0TXIE;						// Enable data interrupt
	UCB0I2CIE |= UCNACKIE | UCSTPIE;		// Enable NACK and stop interrupts

	// Wait until the operation is complete
	do
		LPM0;
	while (UCB0STAT & UCBBUSY);

	// Reset USCI_B0 for next operation
	UCB0CTL1 = UCSSEL_3 | UCSWRST;

	// Error if bytes not written?
	return -(signed) i2c_count;
}

// Read bytes from a device with optional register
int i2c_read(unsigned address, signed reg, unsigned count, char* buffer) {

	// Error if i2c_read called and module not in reset state
	if (!UCB0CTL1 & UCSWRST)
		return -1;

	// Error if buffer is null or count is zero
	if (!count || !buffer)
		return -1;

	// Set state for interrupt
	i2c_data = buffer;
	i2c_count = count;

	UCB0I2CSA = address;

	// Start, send register address if appropriate
	if (reg >= 0) {
		i2c_state = REG;
		UCB0CTL1 = UCSSEL_3 | UCTR | UCTXSTT;
		while (!IFG2 & UCB0TXIFG)
			;
		UCB0TXBUF = reg;
	}

	// TODO: wait for reg if applicable, and THEN...
	// So this is a thing where I took out the relevant part of the TX
	// interrupt, but then failed to replace its functionality. What needs
	// to happen is for the TX interrupt flag to indicate that this byte
	// (the register address) is being transmitted, and then reissue the
	// start condition and join the previous no-register code below. I took
	// out the status variable that had a direction flag... but something
	// like that is needed. So.. yeah. Resume from there.

	UCB0CTL1 = UCSSEL_3 | UCTXSTT;
	UCB0I2CIE |= UCNACKIE | UCSTPIE;
	// FIXME NOT DONE!!!!!!!!!

	// Enable interrupts (also enable TX in for register case)
	IE2 |= UCB0RXIE;

	// If only one byte desired, we have to poll UCTXSTT until address is done
	if (count == 1) {
		while (UCB0CTL1 & UCTXSTT)
			;
		UCB0CTL1 |= UCTXSTP;
	}

	// Wait until completed
	do
		LPM0;
	while (UCB0STAT & UCBBUSY);

	// Reset USCI_B0 for next operation
	UCB0CTL1 = UCSSEL_3 | UCSWRST;

	// Return error if there was a NACK
	return -(signed) i2c_count;
}

// Interrupt service routines
inline void USCI_B0_I2C_STATUS_ISR() {
	if (UCB0STAT & UCNACKIFG) {			// If NACK detected
		UCB0CTL1 |= UCTXSTP;			// Send stop condition
		UCB0STAT &= ~UCNACKIFG;			// Lower interrupt flag
		IE2 &= ~UCB0TXIE;				// Disable interrupts? (TODO: more?)
	}
}

inline void USCI_B0_I2C_TX_RX_ISR() {

	if (IFG2 & UCB0TXIFG) {				// Begin TX interrupt service.
		if (i2c_count) {				// If data remains,
			--i2c_count;				// decrement the byte counter
			UCB0TXBUF = *i2c_data++;	// and send the data.
		} else {						// If no data remains,
			UCB0CTL1 |= UCTXSTP;		// transmit the stop condition, and
			IE2 &= ~UCB0TXIE;			// disable the TX interrupt to end.
		}								// NB: Don't underflow i2c_count
	} else if (IFG2 & UCB0RXIFG) {		// Begin RX interrupt service.
		if (i2c_count) {				// If count is greater than 0,
			--i2c_count;				// decrement count
			*i2c_data++ = UCB0RXBUF;	// and copy data.
		} else {						// If an extra byte is read, just do a
			UCB0RXBUF;					// dummy read to clear the interrupt.
		}								// Stop doesn't signal until the next
		if (i2c_count <= 1)				// byte, so if one byte left (or less),
			UCB0CTL1 |= UCTXSTP;		// stop. An extra byte will come for
	}									// one-byte reads, but we ignore it.
}
