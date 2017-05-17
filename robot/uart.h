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

// Read data
int bytes_pending();
char get_char();
int get_int();
unsigned get_uint();
long get_long();
unsigned long get_ulong();

// Send data
void put_char(char c);
void put_int(int i);
void put_uint(unsigned u);
void put_long(long x);

#endif /* UART_H_ */
