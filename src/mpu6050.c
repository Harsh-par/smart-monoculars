#include "mpu6050.h"

mpu6050_t  mpu6050;

static inline void i2c_write_register(uint32_t i2c_clk, uint8_t reg, uint8_t value)
{
    uint8_t buffer[2] = { reg, value };

    i2c_init(MPU6050_I2C_DEVICE, ADDR_I2C_MPU6050, MPU6050_ADDR_WIDTH, i2c_clk);
    i2c_send_data(MPU6050_I2C_DEVICE, buffer, sizeof(buffer));
}

static inline void i2c_read_register(uint32_t i2c_clk, uint8_t reg, uint8_t *data, size_t length)
{
    i2c_init(MPU6050_I2C_DEVICE, ADDR_I2C_MPU6050, MPU6050_ADDR_WIDTH, i2c_clk);
    i2c_recv_data(MPU6050_I2C_DEVICE, &reg, sizeof(reg), data, length);
}

void mpu6050_init(mpu6050_t *mpu6050, uint32_t i2c_clk, uint8_t i2c_sda, uint8_t i2c_scl)
{
    mpu6050->i2c_clk = i2c_clk;
    mpu6050->i2c_sda = i2c_sda;
    mpu6050->i2c_scl = i2c_scl;

    mpu6050->kalman_pitch_uncertainty = 4 * 4;
    mpu6050->kalman_roll_uncertainty  = 4 * 4;
    
    mpu6050->ax_offset = 0; mpu6050->ay_offset = 0; mpu6050->az_offset = 0;
    mpu6050->gx_offset = 0; mpu6050->gy_offset = 0; mpu6050->gz_offset = 0;

    sysctl_clock_enable(SYSCTL_CLOCK_I2C1);
    
    fpioa_set_function(i2c_sda, FUNC_I2C1_SDA);
    fpioa_set_function(i2c_scl, FUNC_I2C1_SCLK);

    i2c_write_register(i2c_clk, ADDR_REG_PWR_MGMT_1, 0x00);
    
    msleep(50);

    i2c_write_register(i2c_clk, ADDR_REG_PWR_MGMT_2,   0x00);
    i2c_write_register(i2c_clk, ADDR_REG_CONFIG,       0x05);
    i2c_write_register(i2c_clk, ADDR_REG_CONFIG_GYRO,  0x00);
    i2c_write_register(i2c_clk, ADDR_REG_CONFIG_ACCEL, 0x10);
}

void mpu6050_read(mpu6050_t *mpu6050)
{
    uint8_t buffer[14] = {0};

    i2c_read_register(mpu6050->i2c_clk, ADDR_REG_OUT_ACCEL, buffer, sizeof(buffer));
    
    int16_t raw_ax = (buffer[0] << 8) | buffer[1];
    int16_t raw_ay = (buffer[2] << 8) | buffer[3];
    int16_t raw_az = (buffer[4] << 8) | buffer[5];

    int16_t raw_gx = (buffer[8]  << 8) | buffer[9];
    int16_t raw_gy = (buffer[10] << 8) | buffer[11];
    int16_t raw_gz = (buffer[12] << 8) | buffer[13];

    mpu6050->ax = ((float)raw_ax) / 4096.0f - mpu6050->ax_offset;
    mpu6050->ay = ((float)raw_ay) / 4096.0f - mpu6050->ay_offset;
    mpu6050->az = ((float)raw_az) / 4096.0f - mpu6050->az_offset;

    mpu6050->gx = ((float)raw_gx) / 131.0f - mpu6050->gx_offset;
    mpu6050->gy = ((float)raw_gy) / 131.0f - mpu6050->gy_offset;
    mpu6050->gz = ((float)raw_gz) / 131.0f - mpu6050->gz_offset;
}

void mpu6050_compute(mpu6050_t *mpu6050)
{
    mpu6050->angle_roll  = atan2f(mpu6050->ay, sqrtf((mpu6050->ax * mpu6050->ax) + (mpu6050->az * mpu6050->az))) * RADIAN_TO_DEGREE;
    mpu6050->angle_pitch = atan2f(-mpu6050->ax, sqrtf((mpu6050->ay * mpu6050->ay) + (mpu6050->az * mpu6050->az))) * RADIAN_TO_DEGREE;
}

void mpu6050_calibrate(mpu6050_t *mpu6050)
{
    float accelerometer_values[3] = {0, 0, 0};
    float gyroscope_values[3] = {0, 0, 0};

    printf("\n[MPU6050] Calibrating... Keep flat and still.\n");

    for(int i = 0; i < IMU_CALIBRATION_ITERATIONS; i++)
    {
        mpu6050_read(mpu6050);

        accelerometer_values[0] += mpu6050->ax;
        accelerometer_values[1] += mpu6050->ay;
        accelerometer_values[2] += mpu6050->az;

        gyroscope_values[0] += mpu6050->gx;
        gyroscope_values[1] += mpu6050->gy;
        gyroscope_values[2] += mpu6050->gz;

        msleep(1); 
    }

    mpu6050->ax_offset = (accelerometer_values[0] / IMU_CALIBRATION_ITERATIONS);
    mpu6050->ay_offset = (accelerometer_values[1] / IMU_CALIBRATION_ITERATIONS);
    mpu6050->az_offset = (accelerometer_values[2] / IMU_CALIBRATION_ITERATIONS) - 1.0f;

    mpu6050->gx_offset = (gyroscope_values[0] / IMU_CALIBRATION_ITERATIONS);
    mpu6050->gy_offset = (gyroscope_values[1] / IMU_CALIBRATION_ITERATIONS);
    mpu6050->gz_offset = (gyroscope_values[2] / IMU_CALIBRATION_ITERATIONS);

    printf("[MPU6050] Calibration Complete!\n");
}

void mpu6050_filter_pitch(mpu6050_t *mpu6050, const float dt)
{
    float state       = mpu6050->kalman_pitch; 
    float uncertainty = mpu6050->kalman_pitch_uncertainty;
    float input       = mpu6050->gy;
    float measurement = mpu6050->angle_pitch;

    state = state + input * dt;
    uncertainty = uncertainty + 4*4 * dt * dt;

    float kalman_gain = uncertainty / (uncertainty + 3*3);

    state = state + kalman_gain * (measurement - state);
    uncertainty = (1 - kalman_gain) * uncertainty;

    mpu6050->kalman_pitch             = state;
    mpu6050->kalman_pitch_uncertainty = uncertainty;
}

void mpu6050_filter_roll(mpu6050_t *mpu6050, const float dt)
{
    float state       = mpu6050->kalman_roll; 
    float uncertainty = mpu6050->kalman_roll_uncertainty;
    float input       = mpu6050->gx;
    float measurement = mpu6050->angle_roll;

    state = state + input * dt;
    uncertainty = uncertainty + 4*4 * dt * dt;

    float kalman_gain = uncertainty / (uncertainty + 3*3);

    state = state + kalman_gain * (measurement - state);
    uncertainty = (1 - kalman_gain) * uncertainty;

    mpu6050->kalman_roll             = state;
    mpu6050->kalman_roll_uncertainty = uncertainty;
}