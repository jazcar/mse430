/*
 * mse430.h
 *
 *  Created on: Mar 29, 2017
 *      Author: Kristian Sims
 */

#ifndef MSE430_H_
#define MSE430_H_

// Public variables
extern volatile char tick_flag;
extern volatile unsigned long ticks;

// Public function prototypes
void mse430_init();

#endif /* MSE430_H_ */
