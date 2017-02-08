/*
 * uart.c
 *
 *  Created on: Feb 27, 2016
 *      Author: Kristian Sims
 */

#include "uart.h"
#include <msp430.h>
#include "pins.h"

int uart_init() {
	UCA0CTL0 = 0x00;				// UART Mode, defaults
#if CRYSTAL
	UCA0CTL1 |= UCSSEL0 | UCSWRST;	// ACLK, Reset
	UCA0BR0 = 3;
	UCA0MCTL = UCBRS_3;
#else
	UCA0CTL1 |= UCSSEL1 | UCSWRST;	// SMCLK, Reset
	UCA0BR0 = 0x12;
	UCA0BR1 = 0x08;
	UCA0MCTL = UCBRS_6;
#endif
	UCA0CTL1 &= ~UCSWRST;			// Start

	return 0;
}

int uart_getchar(void)
{
    int chr = -1;

    while (!(IFG2 & UCA0RXIFG));
    chr = UCA0RXBUF;

    return chr;
}

int uart_putchar(char c)
{
    /* Wait for the transmit buffer to be ready */
    while (!(IFG2 & UCA0TXIFG));

    /* Transmit data */
    UCA0TXBUF = c;

    return 0;
}
