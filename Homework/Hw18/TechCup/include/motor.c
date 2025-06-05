#include "motor.h"

/*
 * PWM functions
 */ 


void pwm_init_1(unsigned char pin) {
    // Configure the PWM
    gpio_set_function(pin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Get PWM slice number
    float div = 255; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(pin, (uint16_t) wrap * 0.0f); // set the duty cycle to 0
}

void set_pwm(unsigned char pin, float duty_cycle) {
    // Set the PWM duty cycle for motor control
    if (duty_cycle < 0.0f) {
        duty_cycle = 0.0f; // Ensure duty cycle is not negative
    } else if (duty_cycle > 1.0f) {
        duty_cycle = 1.0f; // Ensure duty cycle does not exceed 1.0
    }

    pwm_set_gpio_level(pin, (uint16_t) wrap * duty_cycle); // set the duty cycle to 0
}




/*
 * Motor Functions
 */ 
void motor_init(unsigned char phase, unsigned char speed) {
    // Initialize the speed pin as a pwm pin
    pwm_init_1(speed);
    // Cinfigure the direction pin
    gpio_init(phase);
    gpio_set_dir(phase, GPIO_OUT);
    gpio_put(phase, 0); // Set the direction pin low
    
}


void motor_set_speed (unsigned char dir_pin, unsigned char speed_pin, float speed, bool dir) {
    // Set the speed of the motor
    // This is a placeholder function; actual implementation will depend on the hardware
    if (speed < 0.0f) {
        speed = 0.0f; // Ensure duty cycle is not negative
    } else if (speed > 1.0f) {
        speed = 1.0f; // Ensure duty cycle does not exceed 1.0
    }


    if (dir) {
        gpio_put(dir_pin, 1); // Set the direction pin high
    } else {
        gpio_put(dir_pin, 0); // Set the direction pin low
    }

    set_pwm(speed_pin, speed);
}