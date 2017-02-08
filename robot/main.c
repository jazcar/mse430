#include <msp430.h> 
#include "pins.h"
#include "uart.h"
#include "motor.h"

/*
 * main.c
 */

void wait(int s) {
	while (s-- > 0) {
		__delay_cycles(16000000);
	}
}

union message {
	char bytes[6];
	struct {
		char pad;
		char head;
		int arg_a;
		int arg_b;
	} command;
} message;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	// Init clock - 16 MHz
	DCOCTL = DCO2 | DCO1 | DCO0;
	BCSCTL1 = XT2OFF | RSEL3 | RSEL2 | RSEL1 | RSEL0;
	// BCSCTL2 defaults to 0, which is what we want
//	BCSCTL3 = XCAP_3;	// Internal 12.5 pF capacitors

	// Port 1 Init
	P1SEL = UART_RX | UART_TX | I2C_SCL | I2C_SDA;
	P1SEL2 = UART_RX | UART_TX | I2C_SCL | I2C_SDA;
	P1DIR = BIT0 | BIT3;
	P1REN = 0;
	P1OUT = 0;
	P1IE = 0; // MOTOR_A_HALL_A | MOTOR_A_HALL_B;
	P1IES = 0; 	// Uhhh

	// Port 2 Init
	P2SEL = 0;
	P2SEL2 = 0;
	P2DIR = MOTOR_A_IN_1 | MOTOR_A_IN_2 | MOTOR_B_IN_1 | MOTOR_B_IN_2;
	P2OUT = 0;
	P2IE = 0;
	P2IES = 0;	// Uhhh

//	while (BSCTL3 & LFXT1OF);	// Crystal fault trap

	__enable_interrupt();

	// Start UART
	uart_init();

	// Start motor timers
	motor_init();

//	long save_a=0, save_b=0;
//	while(1) {
//		set_motor_a_power(150);
//		while(motor_a_count < (save_a+12));
//		set_motor_a_power(0);
//		save_a = motor_a_count;
//		set_motor_b_power(-150);
//		while(motor_b_count > (save_b-12));
//		set_motor_b_power(0);
//		save_b = motor_b_count;
//		__delay_cycles(16000);
//	}


	// Test motors
	while (1) {

		while((message.bytes[1] = uart_getchar())!='P');
		message.bytes[2] = uart_getchar();
		message.bytes[3] = uart_getchar();
		message.bytes[4] = uart_getchar();
		message.bytes[5] = uart_getchar();

		set_motor_a_power(message.command.arg_a);
		set_motor_b_power(message.command.arg_b);

	}

	// accel_init();

}
