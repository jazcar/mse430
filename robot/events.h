/*
 * events.h
 *
 *  Created on: Mar 30, 2017
 *      Author: Kristian Sims
 */

#ifndef EVENTS_H_
#define EVENTS_H_


//******************************************************************************
// Service routine events (ordinal):
//	unused				0x0u
//	unused				0x1u
//	unused				0x2u
//	unused				0X3u
//	unused				0x4u
//	unused				0x5u
//	unused				0x6u
//	unused				0x7u
//	unused				0x8u
//	unused				0x9u
//	unused				0xAu
//	unused				0xBu
//	unused				0xCu
//	unused				0xDu
//	unused				0xEu
//	unused				0xFu

#define BIT(x)			(0x1u<<(x))

extern volatile unsigned sys_event;

void events_service();

#endif /* EVENTS_H_ */
