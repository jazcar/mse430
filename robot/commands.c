/*
 * commands.c
 *
 *  Created on: May 11, 2017
 *      Author: Kristian Sims
 */

#include "commands.h"
#include <msp430.h>
#include "mse430.h"
#include "uart.h"
#include "motor.h"
#include "speed.h"

extern long k_p, k_i, k_d, int_cap, int_dom, max_speed;	// speed.c

#define NUM_PARAMS 6
struct {
	unsigned key;
	long* data_ptr;
} params[NUM_PARAMS] = {
		{ .key = 'kp', .data_ptr = &k_p },
		{ .key = 'ki', .data_ptr = &k_i },
		{ .key = 'kd', .data_ptr = &k_d },
		{ .key = 'ic', .data_ptr = &int_cap },
		{ .key = 'id', .data_ptr = &int_dom },
		{ .key = 'ms', .data_ptr = &max_speed },
};
// These are multi-char literals. Not safe for portability across
// systems with different endianness, but here we've hard-coded it
// into the protocol anyway, so whatever.

// Internal function for finding params in list
long* resolve_param(unsigned key) {
	unsigned dex = NUM_PARAMS;
	while (dex-- > 0)
		if (key == params[dex].key)
			return params[dex].data_ptr;
	return 0;
}

// Get a parameter
// In: 'k', two_char_name
// Out: 'k', long_value
void command_get_param() {
	long* param = resolve_param(get_uint());
	if (param) {
		put_char('k');
		put_long(*param);
	} else {
		put_char('E');
	}
}

// Set a parameter
// In: 'K', two_char_name, long_value
// Out: 'K' | 'E'
// Returns 'E' if the parameter name is not recognized
void command_set_param() {
	long* param = resolve_param(get_uint());

	if (param) {
		*param = get_long();
		put_char('K');
	} else {
		put_char('E');
	}
}

// Set motor power values
// In: 'P', power_a, power_b
// Out: 'P'
void command_set_power() {
	motor_a_set_power(get_int());
	motor_b_set_power(get_int());
	controller_on = 0;
	put_char('P');
}

// Get motor power values
// In: 'p'
// Out: 'p', power_a, power_b
void command_get_power() {
	put_char('p');
	put_int(motor_a_power);
	put_int(motor_b_power);
}

// Set speed
// In: 'S', speed_a, speed_b
// Out: 'S'
void command_set_speed() {
	speed_a_set_target(get_int());
	speed_b_set_target(get_int());
	controller_on = 1;
	put_char('S');
}

// Get speed
// In: 's'
// Out: 's', speed_a, speed_b
void command_get_speed() {
	put_char('s');
	put_int((int)motor_a_rate);
	put_int((int)motor_b_rate);
}

void command_event() {
	static char command = 0;

	// Get the command tag (skip if re-entering)
	if (command == 0)
		if (bytes_pending() > 0)
			command = get_char();
		else
			return;

	switch (command) {
	case 'P':	// Set power
		if (bytes_pending() < 4) return;
		command_set_power();
		break;
	case 'p':	// Get power
		command_get_power();
		break;
	case 'S':	// Set speed targets
		if (bytes_pending() < 4) return;
		command_set_speed();
		break;
	case 's':	// Get speed values
		command_get_speed();
		break;
	case 'K':	// Set parameter
		if (bytes_pending() < 6) return;
		command_set_param();
		break;
	case 'k':	// Get parameter
		command_get_param();
		break;
	default:	// Bad Command
		put_char('?');
		break;
	}
	command = 0;
}
