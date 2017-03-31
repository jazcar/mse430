/*
 * motor.c
 *
 *  Created on: Apr 13, 2016
 *      Author: Kristian Sims
 */

#include <msp430.h>
#include "mse430.h"
#include "events.h"

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

	// Event service routine loop
	while (1) {

		// Disable interrupts to avoid race conditions
		__disable_interrupt();

		WATCHDOG;

		// Check if any events are pending
		if (sys_event) {
			__enable_interrupt();
		} else {
			__bis_SR_register(LPM0_bits + GIE);
			continue;  // Check again in case of spurious wakeups
		}

		events_service();
	}

}
