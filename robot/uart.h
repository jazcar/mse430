/*
 * uart.h
 *
 *  Created on: Apr 13, 2016
 *      Author: Kristian Sims
 */

#ifndef UART_H_
#define UART_H_

#include "IObuffer.h"

// Public function prototypes
int uart_init();

// IObuffers
extern IObuffer* uart_rx_buf;
extern IObuffer* uart_tx_buf;

#endif /* UART_H_ */
