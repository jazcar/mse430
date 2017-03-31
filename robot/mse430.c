/*
 * mse430.c
 *
 *  Created on: Mar 29, 2017
 *      Author: Kristian Sims
 */

#include "mse430.h"
#include <msp430.h> 
#include "pins.h"
#include "uart.h"
#include "motor.h"

void mse430_clock_init();

void mse430_init() {

	// Initialize modules
	mse430_clock_init();
	uart_init();
	motor_init();
	// pid_init();
	// accel_init();

	__enable_interrupt();

	// Other miscellaneous stuff
	P1DIR |= RED_LEDS;
#ifdef BUTTON                   // TODO: Fix ISRs so this can work
	P1REN |= BUTTON_1;
	P1IE |= BUTTON_1;			// MOTOR_A_HALL_A | MOTOR_A_HALL_B;
	P1IES |= BUTTON_1; 			// Falling edge
	P1IFG &= ~BUTTON_1;			// Clear spurious interrupts
#endif

	// Unused pins/ports
	P1SEL |= I2C_SCL | I2C_SDA;
	P1SEL2 |= I2C_SCL | I2C_SDA;
	P3DIR = 0xFF;
	P3OUT = 0x00;
}

void mse430_clock_init() {

    // Setup watchdog timer
    WDTCTL = WDT_ADLY_16;
	IE1 |= WDTIE;

	// Init clock - 16 MHz
	DCOCTL = DCO2 | DCO1 | DCO0;
	BCSCTL1 = XT2OFF | RSEL3 | RSEL2 | RSEL1 | RSEL0;
	// BCSCTL2 defaults to 0, which is what we want
	BCSCTL3 = XCAP_3;	// Internal 12.5 pF capacitors

	// Set up 32.768 kHz crystal
	P2SEL |= XIN | XOUT;
	P2DIR |= XIN | XOUT;

	// Crystal fault trap/wait
	while (BCSCTL3 & LFXT1OF)
		__bis_SR_register(LPM0_bits);	// WDT will wake it up
	IFG1 &= ~OFIFG;						// Clear oscillator interrupt flag
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI_TX_ISR() {
	if (IFG2 & UCA0TXIFG) {
		if (uart_tx_buf->count > 0) {			// If byte is available
			char c;
			IOgetc(&c, uart_tx_buf);
			UCA0TXBUF = (unsigned char) c;		// Load next byte
		} else {								// Otherwise
			IE2 &= ~UCA0TXIE;					// Shut off interrupt
		}
	}
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI_RX_ISR() {
	if (IFG2 & UCA0RXIFG) {
		IOputc(UCA0RXBUF, uart_rx_buf);			// Copy into buffer
	}
	__bic_SR_register_on_exit(LPM0_bits);
}

#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR() {
	motor_update_rates();
    __bic_SR_register_on_exit(LPM0_bits);
}
