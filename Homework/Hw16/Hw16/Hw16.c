#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "math.h"

#define MOTOR1_EN 15
#define MOTOR1_PH 14


int main()
{
    stdio_init_all();
    // Wait for USB connection before proceeding
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    // Configure the PWM
    gpio_set_function(MOTOR1_EN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(MOTOR1_EN); // Get PWM slice number
    float div = 255; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 11765; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(MOTOR1_EN, wrap * 0.0f); // set the duty cycle to 0

    // Cinfigure the direction pin
    gpio_init(MOTOR1_PH);
    gpio_set_dir(MOTOR1_PH, GPIO_OUT);
    gpio_put(MOTOR1_PH, -1); // Set the direction pin high

    // Initial message for the user
    printf("Change the motor veolcity using the '+' and '-'keys.\r\n");
    printf("Motor speeds lie in the range of -100 to 100. \r\n");
    printf("Use '+' to increase speed by 1 and '-' to decrease the speed by 1. \r\n");

    unsigned char cmd; 
    float speed = 1.0f; // Initial speed set to 100%
    uint8_t dir = 1; // 1 for forward, -1 for backward
    // Main loop
    while (true) {
        // Read character from the user 
        scanf(" %c", &cmd); // Note the space to skip whitespace

        // Case switch statment for handeling input
        switch (cmd) {
            case 43:
                speed += 0.01f; // Increase speed by 10%
                if (speed > 2.0f) speed = 2.0f; // Cap at 100%
                break;
            case 95:
                speed -= 0.01f; // Decrease speed by 10%
                if (speed < 0.0f) speed = 0.0f; // Cap at -100%
                break;
            default:
                printf("Invalid command. Use '+', '-'.\r\n");
                continue; // Skip the rest of the loop for invalid commands
        }
        
        // Set the PWM duty cycle based on the speed
        if (speed >= 0){
            gpio_put(MOTOR1_PH, 1); // Forward
        } else {
            gpio_put(MOTOR1_PH, 0); // Reverse
        }

        printf("PWM value: %d\r\n", (uint16_t)(wrap * fabsf(1.0f - speed)));
        pwm_set_gpio_level(MOTOR1_EN, (uint16_t) wrap * fabs(1.0f - speed)); // set the duty cycle to 0
        gpio_put(MOTOR1_PH, dir); // Set the direction pin based on the speed
        printf("Motor speed set to: %0.2f \r\n", (1.0f - speed) * 100);

        sleep_ms(100); // Delay to avoid flooding the console
    }
}
