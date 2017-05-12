/*
 * speed.c
 *
 *  Created on: Mar 30, 2017
 *      Author: Kristian Sims
 */

#include "speed.h"
#include <msp430.h>
#include "mse430.h"
#include "motor.h"

#define GLOBAL_IQ 10
#include "IQmathLib.h"

volatile long speed_a_target = 0;
volatile long speed_b_target = 0;

long k_p = _IQ(12.0);
long k_i = _IQ(0.5);
long k_d = _IQ(0.0);
long int_cap = 1000000000;
long int_dom = 0;
long max_speed = 64;

void speed_controller_tick() {

	static long old_a_err = 0;
	static long old_b_err = 0;
	static long err_a_int = 0;
	static long err_b_int = 0;
    long err_a_der, err_b_der;
	long speed_a_err = speed_a_target - motor_a_rate;
	long speed_b_err = speed_b_target - motor_b_rate;

	err_a_der = speed_a_err - old_a_err;
	err_b_der = speed_b_err - old_b_err;

    old_a_err = speed_a_err;
    old_b_err = speed_b_err;

	err_a_int += speed_a_err;
	err_b_int += speed_b_err;

	if (int_cap > 0) {
	    err_a_int = long_constrain(err_a_int, -int_cap, int_cap);
	    err_b_int = long_constrain(err_b_int, -int_cap, int_cap);
	}

	motor_a_set_power(
	        _IQmpyI32int(k_p, speed_a_err) +
	        _IQmpyI32int(k_i, err_a_int) +
	        _IQmpyI32int(k_d, err_a_der) );
	motor_b_set_power(
	        _IQmpyI32int(k_p, speed_b_err) +
	        _IQmpyI32int(k_i, err_b_int) +
	        _IQmpyI32int(k_d, err_b_der) );
}

void speed_a_set_target(int val) {

    if (max_speed > 0)
        speed_a_target = long_constrain((long)val, -max_speed, max_speed);
    else
        speed_a_target = (long)val;
}

void speed_b_set_target(int val) {

	if (max_speed > 0)
		speed_b_target = long_constrain((long)val, -max_speed, max_speed);
	else
		speed_b_target = (long)val;
}
