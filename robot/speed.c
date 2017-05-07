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

const int max_speed = 1000;         // TODO: Should this be const?

volatile int speed_a_target = 0;
volatile int speed_b_target = 0;

#define FIXED_POINT_OFFSET 8

int k_prop = 1 << FIXED_POINT_OFFSET;
int k_der = 0 << FIXED_POINT_OFFSET;
//int integral_coefficient = 0;

void speed_controller_tick() {

	static int old_a_err = 0;
	static int old_b_err = 0;

	int speed_a_err = speed_a_target - motor_a_rate;
	int speed_b_err = speed_b_target - motor_b_rate;

	int err_a_der = old_a_err - speed_a_err;
	int err_b_der = old_b_err - speed_b_err;

	old_a_err = speed_a_err;
	old_b_err = speed_b_err;

	long temp;
	temp = k_prop * speed_a_err + k_der * err_a_der;
	int power_a = (int)(temp >> FIXED_POINT_OFFSET);
	motor_a_set_power(power_a);
	temp = k_prop * speed_b_err + k_der * err_b_der;
	int power_b = (int)(temp >> FIXED_POINT_OFFSET);
	motor_b_set_power(power_b);
}

void speed_a_set_target(int val) {

    if (val > max_speed)
    	speed_a_target = max_speed;
    else if (val < -max_speed)
    	speed_a_target = -max_speed;
    else
    	speed_a_target = val;
}

void speed_b_set_target(int val) {

    if (val > max_speed)
    	speed_b_target = max_speed;
    else if (val < -max_speed)
    	speed_b_target = -max_speed;
    else
    	speed_b_target = val;
}
