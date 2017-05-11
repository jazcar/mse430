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

#define GLOBAL_Q 8
#include "QmathLib.h"

volatile int speed_a_target = 0;
volatile int speed_b_target = 0;

int k_p = _Q(1.0);
int k_i = _Q(0.1);
int k_d = _Q(0.1);
int int_cap = 0;
int max_speed = 0;

void speed_controller_tick() {

	static int old_a_err = 0;
	static int old_b_err = 0;
	static int err_a_int = 0;
	static int err_b_int = 0;
    int err_a_der, err_b_der;
	int speed_a_err = speed_a_target - motor_a_rate;
	int speed_b_err = speed_b_target - motor_b_rate;

	err_a_der = old_a_err - speed_a_err;
	err_b_der = old_b_err - speed_b_err;

    old_a_err = speed_a_err;
    old_b_err = speed_b_err;

	err_a_int += speed_a_err;
	err_b_int += speed_b_err;

	if (int_cap > 0) {
	    err_a_int = constrain(err_a_int, -int_cap, int_cap);
	    err_b_int = constrain(err_b_int, -int_cap, int_cap);
	}

	motor_a_set_power(
	        _QmpyI16int(k_p, speed_a_err) +
	        _QmpyI16int(k_i, err_a_int) +
	        _QmpyI16int(k_d, err_a_der) );
	motor_b_set_power(
	        _QmpyI16int(k_p, speed_b_err) +
	        _QmpyI16int(k_i, err_b_int) +
	        _QmpyI16int(k_d, err_b_der) );
}

void speed_a_set_target(int val) {

    if (max_speed > 0)
        speed_a_target = constrain(val, -max_speed, max_speed);
    else
        speed_a_target = val;
    controller_on = 1;
}

void speed_b_set_target(int val) {

    speed_b_target = constrain(val, -max_speed, max_speed);
    controller_on = 1;
}
