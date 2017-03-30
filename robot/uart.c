/*
 * uart.c
 *
 *  Created on: Feb 27, 2016
 *      Author: Kristian Sims
 */

#include "uart.h"
#include <msp430.h>
#include "pins.h"
#include "IObuffer.h"

IObuffer* uart_rx_buf, uart_tx_buf;

int uart_init() {

	// Create IObuffers (probably quite oversized)
	uart_rx_buf = IObuffer_create(64);
	uart_tx_buf = IObuffer_create(64);

	// Set up USCIA0
	UCA0CTL0 = 0x00;				// UART Mode, defaults

#ifdef UART_CRYSTAL                 // (Don't) use crystal for clock
	UCA0CTL1 |= UCSSEL0 | UCSWRST;	// ACLK, Reset
	UCA0BR0 = 3;
	UCA0MCTL = UCBRS_3;
#else                               // Set timing with DCO (less error)
	UCA0CTL1 |= UCSSEL1 | UCSWRST;	// SMCLK, Reset
	UCA0BR0 = 0x12;
	UCA0BR1 = 0x08;
	UCA0MCTL = UCBRS_6;
#endif

	UCA0CTL1 &= ~UCSWRST;			// Start USCIA0
	IE2 |= UCA0RXIE;                // Enable RX interrupt
	return 0;
}

void uart_tx_callback() {
	IE2 |= UCA0TXIE;	// Enable TX interrupt (triggers immediately)
}

void uart_rx_callback() {
	sys_event |= BIT(CMD_EVENT);	// Signal command event
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCI_TX_ISR() {
	if (IFG2 & UCA0TXIFG) {
		if (uart_tx_buf->count > 0) {			// If byte is available
			IOgetc(&UCA0TXBUF, uart_rx_buf);	// Load next byte
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
}
