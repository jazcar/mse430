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
#include "speed.h"

extern int k_p, k_i, k_d, int_cap, int_dom, max_speed;	// speed.c

#define NUM_PARAMS 6
struct {
	union {
		char key_chars[2];
		unsigned key_int;
	};
	int* data_ptr;
} params[NUM_PARAMS] = {
		{ .key_chars[0]='k', .key_chars[1]='p', .data_ptr = &k_p },
		{ .key_chars[0]='k', .key_chars[1]='i', .data_ptr = &k_i },
		{ .key_chars[0]='k', .key_chars[1]='d', .data_ptr = &k_d },
		{ .key_chars[0]='i', .key_chars[1]='c', .data_ptr = &int_cap },
		{ .key_chars[0]='i', .key_chars[1]='d', .data_ptr = &int_dom },
		{ .key_chars[0]='m', .key_chars[1]='s', .data_ptr = &max_speed },
};

void set_param() {
	union {
		char bytes[2];
		unsigned uint16;
		int int16;
	} key, value;
	unsigned dex = NUM_PARAMS;

	IOgetc(&key.bytes[0], uart_rx_buf);
	IOgetc(&key.bytes[1], uart_rx_buf);
	IOgetc(&value.bytes[0], uart_rx_buf);
	IOgetc(&value.bytes[1], uart_rx_buf);

	while (dex-- > 0) {
		if (key.uint16 == params[dex].key_int) {
			*params[dex].data_ptr = value.int16;
			return;
		}
	}
	// Error
}
