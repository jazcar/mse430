/*
 * motor.c
 *
 *  Created on: Apr 13, 2016
 *      Author: Kristian Sims
 */

#include <msp430.h>
#include "mse430.h"
#include "uart.h"
#include "motor.h"
#include "speed.h"
#include "commands.h"

union message {
	char bytes[4];
	struct {
		int arg_a;
		int arg_b;
	} command;
} message;

int main(void) {

	// Initialize everything
	mse430_init();

	// Loop forever
	while (1) {

		// Avoid nasty race conditions
		__disable_interrupt();

		if (tick_flag) {
			__enable_interrupt();

			// Run speed controller
			tick_flag = 0;
			if (controller_on)
			    speed_controller_tick();

			// Send speed back to remote
			message.command.arg_a = (int)motor_a_rate;
			message.command.arg_b = (int)motor_b_rate;
			IOputc('S', uart_tx_buf);
			IOnputs(message.bytes, 4, uart_tx_buf);

		} else if (uart_rx_buf->count >= 5) {
			__enable_interrupt();
			command_event();

		} else {
			__bis_SR_register(LPM0_bits | GIE);

		}
	}
}
