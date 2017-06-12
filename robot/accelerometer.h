/*
 * accelerometer.h
 *
 *  Created on: Jun 6, 2017
 *      Author: Kristian Sims
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

// Public function prototypes
void accelerometer_init();
void accelerometer_sleep();
void accelerometer_raw_accel_xy(char*);
void accelerometer_raw_gyro_xy(char*);

/* Filter configurations. */
enum lpf_e {
    INV_FILTER_256HZ_NOLPF2 = 0,
    INV_FILTER_188HZ,
    INV_FILTER_98HZ,
    INV_FILTER_42HZ,
    INV_FILTER_20HZ,
    INV_FILTER_10HZ,
    INV_FILTER_5HZ,
    INV_FILTER_2100HZ_NOLPF,
    NUM_FILTER
};

/* Full scale ranges. */
enum gyro_fsr_e {
    INV_FSR_250DPS = 0,
    INV_FSR_500DPS,
    INV_FSR_1000DPS,
    INV_FSR_2000DPS,
    NUM_GYRO_FSR
};

/* Full scale ranges. */
enum accel_fsr_e {
    INV_FSR_2G = 0,
    INV_FSR_4G,
    INV_FSR_8G,
    INV_FSR_16G,
    NUM_ACCEL_FSR
};

/* Clock sources. */
enum clock_sel_e {
    INV_CLK_INTERNAL = 0,
    INV_CLK_PLL,
    NUM_CLK
};


#define MPU9250_ADR		0x68		// Gyro/Accelerometer

#define BIT_FIFO_EN         (0x40)
#define BIT_DMP_EN          (0x80)
#define BIT_FIFO_RST        (0x04)
#define BIT_DMP_RST         (0x08)
#define BIT_FIFO_OVERFLOW   (0x10)
#define BIT_DATA_RDY_EN     (0x01)
#define BIT_DMP_INT_EN      (0x02)
#define BIT_MOT_INT_EN      (0x40)
#define BITS_FSR            (0x18)
#define BITS_LPF            (0x07)
#define BITS_HPF            (0x07)
#define BITS_CLK            (0x07)
#define BIT_FIFO_SIZE_1024  (0x40)
#define BIT_FIFO_SIZE_2048  (0x80)
#define BIT_FIFO_SIZE_4096  (0xC0)
#define BIT_RESET           (0x80)
#define BIT_SLEEP           (0x40)
#define BIT_S0_DELAY_EN     (0x01)
#define BIT_S2_DELAY_EN     (0x04)
#define BITS_SLAVE_LENGTH   (0x0F)
#define BIT_SLAVE_BYTE_SW   (0x40)
#define BIT_SLAVE_GROUP     (0x10)
#define BIT_SLAVE_EN        (0x80)
#define BIT_I2C_READ        (0x80)
#define BITS_I2C_MASTER_DLY (0x1F)
#define BIT_AUX_IF_EN       (0x20)
#define BIT_ACTL            (0x80)
#define BIT_LATCH_EN        (0x20)
#define BIT_ANY_RD_CLR      (0x10)
#define BIT_BYPASS_EN       (0x02)
#define BITS_WOM_EN         (0xC0)
#define BIT_LPA_CYCLE       (0x20)
#define BIT_STBY_XA         (0x20)
#define BIT_STBY_YA         (0x10)
#define BIT_STBY_ZA         (0x08)
#define BIT_STBY_XG         (0x04)
#define BIT_STBY_YG         (0x02)
#define BIT_STBY_ZG         (0x01)
#define BIT_STBY_XYZA       (BIT_STBY_XA | BIT_STBY_YA | BIT_STBY_ZA)
#define BIT_STBY_XYZG       (BIT_STBY_XG | BIT_STBY_YG | BIT_STBY_ZG)

#define WHO_AM_I 0x75
#define RATE_DIV 0x19
#define LPF 0x1A
#define PROD_ID 0x0C
#define USER_CTRL 0x6A
#define FIFO_EN 0x23
#define GYRO_CFG 0x1B
#define ACCEL_CFG 0x1C
#define MOTION_THR 0x1F
#define MOTION_DUR 0x20
#define FIFO_COUNT_H 0x72
#define FIFO_R_W 0x74
#define RAW_GYRO 0x43
#define RAW_ACCEL 0x3B
#define TEMP 0x41
#define INT_ENABLE 0x38
#define DMP_INT_STATUS 0x39
#define INT_STATUS 0x3A
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
#define INT_PIN_CFG 0x37
#define MEM_R_W 0x6F
#define ACCEL_OFFS 0x06
#define I2C_MST 0x24
#define BANK_SEL 0x6D
#define MEM_START_ADDR 0x6E
#define PRGM_START_H 0x70
#define RAW_COMPASS 0x49
#define YG_OFFS_TC 0x01
#define S0_ADDR 0x25
#define S0_REG 0x26
#define S0_CTRL 0x27
#define S1_ADDR 0x28
#define S1_REG 0x29
#define S1_CTRL 0x2A
#define S4_CTRL 0x34
#define S0_DO 0x63
#define S1_DO 0x64
#define I2C_DELAY_CTRL 0x6

#endif /* ACCELEROMETER_H_ */
