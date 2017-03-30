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

    // Setup watchdog timer
    WDTCTL = WDT_ADLY_16;
	IE1 |= WDTIE;

	// Init clock - 16 MHz
	DCOCTL = DCO2 | DCO1 | DCO0;
	BCSCTL1 = XT2OFF | RSEL3 | RSEL2 | RSEL1 | RSEL0;
	// BCSCTL2 defaults to 0, which is what we want
	BCSCTL3 = XCAP_3;	// Internal 12.5 pF capacitors

	// Port 1 Init
	P1SEL = UART_RX | UART_TX | I2C_SCL | I2C_SDA;
	P1SEL2 = UART_RX | UART_TX | I2C_SCL | I2C_SDA;
	P1DIR = RED_LEDS;
	P1REN = BUTTON_1;
	P1OUT = 0;
#ifdef BUTTON                   // TODO: Fix ISRs so this can work
	P1IE = BUTTON_1;			// MOTOR_A_HALL_A | MOTOR_A_HALL_B;
	P1IES = BUTTON_1; 			// Falling edge
	P1IFG &= ~BUTTON_1;			// Clear spurious interrupts
#endif

	// Port 2 Init
	P2SEL = XIN | XOUT;
	P2SEL2 = 0;
	P2DIR = MOTOR_A_IN_1 | MOTOR_A_IN_2 | MOTOR_B_IN_1 | MOTOR_B_IN_2 | XIN | XOUT;
	P2OUT = 0;

	// Port 3 "Init"
	P3DIR = 0xFF;

	// Crystal fault trap
	while (BCSCTL3 & LFXT1OF)
	    __bis_SR_register(LPM0_bits);
	IFG1 &= ~OFIFG;				// Clear oscillator interrupt flag

	// Start UART
	uart_init();

	// Start motor timers
	motor_init();

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


#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR() {
    __bic_SR_register_on_exit(LPM0_bits);
}
