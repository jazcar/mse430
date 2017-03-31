/*
 * events.c
 *
 *  Created on: Mar 30, 2017
 *      Author: Kristian Sims
 */

#include <msp430.h>
#include "events.h"
#include "mse430.h"

volatile unsigned sys_event = 0;

void log_timestamp();
int interrupt_event();

inline void events_service() {

	// Service events -- Round Robin scheduling

	// Each event subroutine will return zero if the event was processed, or
	// positive non-zero if the event should remain scheduled for the next
	// iteration of the loop. Negative values should be reserved for errors,
	// but at this time events should not return errors, and handle them
	// internally.

	// All device interrupts (shared line)
	if (sys_event & BIT(INT_EVENT))
		if (!interrupt_event())
			sys_event &= ~BIT(INT_EVENT);

	// Process a command token from the buffer
	if (sys_event & BIT(CMD_EVENT))
		if (!cmd_event())
			sys_event &= ~BIT(CMD_EVENT);
}

int interrupt_event() {
	return 0;
}
