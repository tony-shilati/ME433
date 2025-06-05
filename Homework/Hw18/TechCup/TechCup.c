#include <stdio.h>
#include "pico/stdlib.h"
#include "include/motor.h"
#include "include/cam.h"
#include "math.h"

#define MOTOR2_EN 17
#define MOTOR2_PH 16

#define MOTOR1_EN 18
#define MOTOR1_PH 19



int main()
{
    stdio_init_all();

    // Initialize the camera
    init_camera_pins();

    // Initialize the motor
    motor_init(MOTOR1_PH, MOTOR1_EN);
    motor_init(MOTOR2_PH, MOTOR2_EN);

    float base_pwm = 0.35f;
    float control_pwm = 0.19f;

    float u1 = 0.0f; // speed of left motor
    float u2 = 0.0f; // speed of right motor

    while (true) {
        // Capture an image and find com
        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(IMAGESIZEY/2); // find the com of the line

        // double error_func = tanh((double) ((com * 1.0f - (IMAGESIZEX/2)) / 30.0f)) - 0.075; // calculate the error function
        double error_func = pow((com - (IMAGESIZEX/2)) / 20.0f, 2); // calculate the error function


        if (error_func > 1.0f) {
            error_func = 1.0f; // limit the error function to 1.0
        } else if (error_func < -1.0f) {
            error_func = -1.0f; // limit the error function to -1.0
        }

        if (error_func < 0.0f) {
            // Turn right
            u1 = fabs(error_func);
            u2 = -fabs(error_func) * 1.35f; // reduce speed of left motor

        } else if (error_func > 0.0f) {
            // Turn left
            u1 = -fabs(error_func)*1.35f; // reduce speed of right motor
            u2 = fabs(error_func);
            
        } else {
            // Go straight
            u1 = 0.0f;
            u2 = 0.0f;
        }

        float pwm_1 = (1.0f - (base_pwm + (u1 * control_pwm)));
        float pwm_2 = (1.0f - (base_pwm + (u2 * control_pwm)));

        // Ensure PWM values are within bounds
        if (pwm_1 < 0.0f) {
            pwm_1 = 0.0f; // Ensure PWM is not negative
        } else if (pwm_1 > 1.0f) {
            pwm_1 = 1.0f; // Ensure PWM does not exceed 1.0
        }
        if (pwm_2 < 0.0f) {
            pwm_2 = 0.0f; // Ensure PWM is not negative
        } else if (pwm_2 > 1.0f) {
            pwm_2 = 1.0f; // Ensure PWM does not exceed 1.0
        }

        // Set the motor direction based on the image
        motor_set_speed(MOTOR1_PH, MOTOR1_EN, pwm_1, true);
        motor_set_speed(MOTOR2_PH, MOTOR2_EN, pwm_2, true);

        // motor_set_speed(MOTOR1_PH, MOTOR1_EN, 0.25f, true);
        // motor_set_speed(MOTOR2_PH, MOTOR2_EN, 0.25f, true);

    }
}
