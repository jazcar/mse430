/*
 * uart.c
 *
 *  Created on: Feb 27, 2016
 *      Author: Kristian Sims
 */

#include "uart.h"
#include <msp430.h>
#include "pins.h"

IObuffer _uart_rx_buf;
IObuffer _uart_tx_buf;
IObuffer* const uart_rx_buf = &_uart_rx_buf;
IObuffer* const uart_tx_buf = &_uart_tx_buf;
#define UART_BUF_SIZE  32
char uart_rx_chars[UART_BUF_SIZE];
char uart_tx_chars[UART_BUF_SIZE];

void uart_rx_callback();
void uart_tx_callback();

int uart_init() {

	// Create IObuffers (probably quite oversized)
	IObuffer_init(uart_rx_buf, (char*) &uart_rx_chars,
	              UART_BUF_SIZE, uart_rx_callback);
	uart_rx_buf->callback_once = 0;
	IObuffer_init(uart_tx_buf, (char*) &uart_tx_chars,
	              UART_BUF_SIZE, uart_tx_callback);
	uart_tx_buf->callback_once = 1;

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

	P1SEL |= UART_RX | UART_TX;		// Pins in USCI mode
	P1SEL2 |= UART_RX | UART_TX;

	UCA0CTL1 &= ~UCSWRST;			// Start USCIA0
	IE2 |= UCA0RXIE;                // Enable RX interrupt
	return 0;
}

void uart_tx_callback() {
	IE2 |= UCA0TXIE;	// Enable TX interrupt (triggers immediately)
}

void uart_rx_callback() {
	// sys_event |= BIT(CMD_EVENT);	// Signal command event
}

union intbuf {
	char bytes[2];
	unsigned uint16;
	int int16;
};

int get_int() {
	union intbuf buf;
	IOgetc(&buf.bytes[0], uart_rx_buf);
	IOgetc(&buf.bytes[1], uart_rx_buf);
	return buf.int16;
}

unsigned get_uint() {
	union intbuf buf;
	IOgetc(&buf.bytes[0], uart_rx_buf);
	IOgetc(&buf.bytes[1], uart_rx_buf);
	return buf.uint16;
}
