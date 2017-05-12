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
	union {
		char key_chars[2];
		unsigned key_int;
	};
	long* data_ptr;
} params[NUM_PARAMS] = {
		{ .key_chars[0]='k', .key_chars[1]='p', .data_ptr = &k_p },
		{ .key_chars[0]='k', .key_chars[1]='i', .data_ptr = &k_i },
		{ .key_chars[0]='k', .key_chars[1]='d', .data_ptr = &k_d },
		{ .key_chars[0]='i', .key_chars[1]='c', .data_ptr = &int_cap },
		{ .key_chars[0]='i', .key_chars[1]='d', .data_ptr = &int_dom },
		{ .key_chars[0]='m', .key_chars[1]='s', .data_ptr = &max_speed },
};

void set_param(unsigned key, int value) {
	unsigned dex = NUM_PARAMS;

	while (dex-- > 0) {
		if (key == params[dex].key_int) {
			*params[dex].data_ptr = (long)value;
			return;
		}
	}
	// No matching key
}

void command_event() {
	char c;
	IOgetc(&c, uart_rx_buf);
	switch(c) {
	unsigned key;
	int value;
	case 'P':
		motor_a_set_power(get_int());
		motor_b_set_power(get_int());
		controller_on = 0;
		break;
	case 'S':
		speed_a_set_target(get_int());
		speed_b_set_target(get_int());
		controller_on = 1;
		break;
	case 'K':
		key = get_uint();
		value = get_int();
		set_param(key, value);
		break;
	default:
		//Error
		break;
	}
}
