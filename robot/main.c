/*
 * motor.c
 *
 *  Created on: Apr 13, 2016
 *      Author: Kristian Sims
 */

#include <msp430.h>
#include "mse430.h"
#include "motor.h"
#include "uart.h"

union message {
	char bytes[4];
	struct {
		int arg_a;
		int arg_b;
	} command;
} message;

int main(void) {

	mse430_init();

	while (1) {

		char c;

		while(IOgetc(&c, uart_rx_buf))
			__bis_SR_register(LPM0_bits+GIE);
		if (c != 'P')
			continue;
		while(IOgetc(&message.bytes[0], uart_rx_buf))
			__bis_SR_register(LPM0_bits+GIE);
		while(IOgetc(&message.bytes[1], uart_rx_buf))
			__bis_SR_register(LPM0_bits+GIE);
		while(IOgetc(&message.bytes[2], uart_rx_buf))
			__bis_SR_register(LPM0_bits+GIE);
		while(IOgetc(&message.bytes[3], uart_rx_buf))
			__bis_SR_register(LPM0_bits+GIE);

		set_motor_a_power(message.command.arg_a);
		set_motor_b_power(message.command.arg_b);
		message.command.arg_a = motor_a_rate;
		message.command.arg_b = motor_b_rate;

		IOnputs(message.bytes, 4, uart_tx_buf);
	}

}
