#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// IMU Address
#define MPU6050_ADDR 0x68 // I2C address of the MPU6050

// IMU config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

// Prototypes
void read_IMU_data(int16_t *data);
void config_IMU();

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Configure the IMU
    config_IMU();

    int16_t data[8]; // Array to store sensor data

    while (true) {
        // Read IMU data
        read_IMU_data(data);

        printf("Who am I: %d \n", data[7]);
        printf("Accel X: %d, Y: %d, Z: %d\n", data[0], data[1], data[2]);
        printf("Gyro X: %d, Y: %d, Z: %d\n", data[4], data[5], data[6]);
        printf("Temperature: %d\n", data[3]);
        sleep_ms(1000);
    }
}

void config_IMU(){

    uint8_t pwr_mgmt1[2] = {PWR_MGMT_1, 0x00}; // Set the power management register to 0x0
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, pwr_mgmt1, 2, false);

    uint8_t accel_config[2] = {ACCEL_CONFIG, 0x00}; // Set sensitivity to +/- 2g
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, accel_config, 2, false);

    uint8_t gyro_config[2] = {GYRO_CONFIG, 0x03}; // Set sensitivity to +/- 2000 degrees/sec
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, gyro_config, 2, false);

}

void read_IMU_data(int16_t *data) {

    uint8_t byte_data[14];
    uint8_t data_addr = ACCEL_XOUT_H;

    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &data_addr, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, byte_data, 14, false);

    data[0] = (byte_data[0] << 8) | byte_data[1]; // X-axis acceleration
    data[1] = (byte_data[2] << 8) | byte_data[3]; // Y-axis acceleration
    data[2] = (byte_data[4] << 8) | byte_data[5]; // Z-axis acceleration
    data[3] = (byte_data[6] << 8) | byte_data[7]; // Temperature
    data[4] = (byte_data[8] << 8) | byte_data[9]; // X-axis gyroscope
    data[5] = (byte_data[10] << 8) | byte_data[11]; // Y-axis gyroscope
    data[6] = (byte_data[12] << 8) | byte_data[13]; // Z-axis gyroscope
    data[7] = byte_data[14]; // WHO_AM_I

}
