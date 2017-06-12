/*
 * accelerometer.c
 *
 *  Created on: Jun 6, 2017
 *      Author: Kristian Sims
 */

#include <msp430.h>
#include "accelerometer.h"
#include "mse430.h"
#include "i2c.h"

/**
 *  Initialize hardware.
 *  Initial configuration:
 *  Gyro FSR: +/- 2000DPS
 *  Accel FSR +/- 2G
 *  DLPF: 42Hz
 *  FIFO rate: 50Hz
 *  Clock source: Gyro PLL
 *  FIFO: Disabled.
 *  Data ready interrupt: Disabled, active low, unlatched.
 */
void accelerometer_init() {
	char data;

	// Reset device
	data = BIT_RESET;
	i2c_write(MPU9250_ADR, 0x6B /*pwr_mgmt_1*/, 1, &data);

	// Wait 100 ms
	wait_ticks(7);

	// Wake up chip
	data = 0x00;
	i2c_write(MPU9250_ADR, 0x6B /*pwr_mgmt_1*/, 1, &data);

	// Wait 10 ms
	wait_ticks(1);

	// Set gyro full-scale range
	data = (INV_FSR_2000DPS << 3) | 0x03; // the 0x03 is to enable the LPF
	i2c_write(MPU9250_ADR, 0x1B /*gyro_cfg*/, 1, &data);

	// Set accelerometer full-scale range
	data = INV_FSR_16G << 3;
	i2c_write(MPU9250_ADR, 0x1C /*accel_cfg*/, 1, &data);

	// Set digital low-pass filter for gyro
	data = INV_FILTER_42HZ;
	i2c_write(MPU9250_ADR, 0x1A /*lpf*/, 1, &data);

	// Set sample rate to 1000 Hz
	data = 1000 / 1000 - 1;
	i2c_write(MPU9250_ADR, 0x19 /*rate_div*/, 1, &data);
	// by default, the SDK would now overwrite the low-pass filter to be 20 Hz,
	// but let's skip this for now...

	// Disable data ready interrupt.
	data = 0; //BIT_DATA_RDY_EN;
	i2c_write(MPU9250_ADR, 0x38 /*int_enable*/, 1, &data);

	// There was the set_bypass here... but that was for offboard sensors, I think

	// Configure sensors... enable gyro and don't disable accel
	data = INV_CLK_PLL;
	i2c_write(MPU9250_ADR, 0x6B /*pwr_mgmt_1*/, 1, &data);

	data = 0x00;
	i2c_write(MPU9250_ADR, 0x6C /*pwr_mgmt_2*/, 1, &data);

	return;
}

void accelerometer_sleep() {
	char data = BIT_SLEEP;	// Reset device
	i2c_write(MPU9250_ADR, 0x6B /*pwr_mgmt_1*/, 1, &data);
	return;
}

void accelerometer_raw_accel_xy(char* buffer) {
	i2c_read(MPU9250_ADR, 0x3B /*accel_xout_h*/, 4, buffer);
	return;
}

void accelerometer_raw_gyro_xy(char* buffer) {
	i2c_read(MPU9250_ADR, 0x43 /*gyro_xout_h*/, 4, buffer);
	return;
}
