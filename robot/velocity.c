/*
 * velocity.c
 *
 *  Created on: Mar 30, 2017
 *      Author: Kristian Sims
 */

#include "velocity.h"
#include <msp430.h>
#include "mse430.h"
#include "motor.h"

const int max_speed = 80;         // TODO: Should this be const?

volatile int velocity_a_target = 0;
volatile int velocity_b_target = 0;

void velocity_controller_tick() {}

void velocity_a_set_target(int vel) {

    if (vel > max_speed)
    	velocity_a_target = max_speed;
    else if (vel < -max_speed)
    	velocity_a_target = -max_speed;
    else
    	velocity_a_target = vel;
}

void velocity_b_set_target(int vel) {

    if (vel > max_speed)
    	velocity_b_target = max_speed;
    else if (vel < -max_speed)
    	velocity_b_target = -max_speed;
    else
    	velocity_b_target = vel;
}
