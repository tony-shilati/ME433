#include <stdio.h>
#include "pico/stdlib.h"
#include "include/motor.h"
#include "include/cam.h"

#define MOTOR1_EN 17
#define MOTOR1_PH 16

#define MOTOR2_EN 18
#define MOTOR2_PH 19



int main()
{
    stdio_init_all();

    // Initialize the camera
    init_camera_pins();

    // Initialize the motor
    motor_init(MOTOR1_PH, MOTOR1_EN);
    motor_init(MOTOR2_PH, MOTOR2_EN);

    // Set the speed of the motors
    motor_set_speed(MOTOR1_PH, MOTOR1_EN, 0.5f, true);
    motor_set_speed(MOTOR2_PH, MOTOR2_EN, 0.5f, true);

    sleep_ms(1000);

    while (true) {
        // Capture an image and find com
        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();

        // Set the motor direction based on the image
        dir = !dir; // Toggle direction
        motor_set_speed(MOTOR1_PH, MOTOR1_EN, 0.5f, dir);
        motor_set_speed(MOTOR2_PH, MOTOR2_EN, 0.5f, dir);

        sleed_ms(1000);
    }
}
