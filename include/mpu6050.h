#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include "fpioa.h"
#include "i2c.h"
#include "sleep.h"
#include "sysctl.h"

#include "constant.h"

#define MPU6050_I2C_DEVICE    1
#define MPU6050_ADDR_WIDTH    7

#define ADDR_I2C_MPU6050      0x68
#define ADDR_REG_PWR_MGMT_1   0x6B
#define ADDR_REG_PWR_MGMT_2   0x6C
#define ADDR_REG_CONFIG       0x1A
#define ADDR_REG_CONFIG_GYRO  0x1B
#define ADDR_REG_CONFIG_ACCEL 0x1C
#define ADDR_REG_OUT_ACCEL    0x3B
#define ADDR_REG_OUT_TEMP     0x41
#define ADDR_REG_OUT_GYRO     0x43

#define IMU_CALIBRATION_ITERATIONS 1000

typedef struct {
    uint32_t i2c_clk;

    float ax; 
    float ay; 
    float az; 
    
    float gx; 
    float gy; 
    float gz;

    float ax_offset; 
    float ay_offset; 
    float az_offset; 

    float gx_offset; 
    float gy_offset; 
    float gz_offset; 

    float angle_roll; 
    float angle_pitch;

    float kalman_roll;
    float kalman_pitch;

    float kalman_roll_uncertainty;
    float kalman_pitch_uncertainty;

    uint8_t i2c_sda;
    uint8_t i2c_scl;

} mpu6050_t;

extern mpu6050_t mpu6050;

void mpu6050_init(mpu6050_t *mpu6050, uint32_t i2c_clk, uint8_t i2c_sda, uint8_t i2c_scl);
void mpu6050_read(mpu6050_t *mpu6050);
void mpu6050_compute(mpu6050_t *mpu6050);
void mpu6050_calibrate(mpu6050_t *mpu6050);
void mpu6050_filter_pitch(mpu6050_t *mpu6050, const float dt);
void mpu6050_filter_roll(mpu6050_t *mpu6050, const float dt);

#endif