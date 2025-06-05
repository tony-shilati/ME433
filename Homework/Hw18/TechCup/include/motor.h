#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

// Constants
 static uint16_t wrap = 11765; // when to rollover, must be less than 65535

// Function prototypes
void pwm_init_1(unsigned char pin);
void set_pwm(unsigned char pin, float duty_cycle);
void motor_init(unsigned char phase, unsigned char speed);
void motor_set_speed (unsigned char dir_pin, unsigned char speed_pin, float speed, bool dir);