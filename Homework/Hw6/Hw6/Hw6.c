#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

// void init_gpio_extension();



int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    /*
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    */
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    gpio_init(15); // Initialize GPIO 15
    gpio_set_dir(15, GPIO_OUT); // Set GPIO 15 as output for heartbeat LED
    gpio_put(15, 1); // Turn on the LED

    uint64_t start_time = to_ms_since_boot(get_absolute_time()); // Get the current time
    while (true) {

        

        if (to_ms_since_boot(get_absolute_time()) - start_time > 500) { // If 0.5 second has passed
            start_time = to_ms_since_boot(get_absolute_time()); // Reset the timer
            gpio_put(15, !gpio_get(15)); // Toggle the LED
        }
    
        // printf("Hello World!\n"); // Print Hello World to the console
        // sleep_ms(500); // Sleep for 100ms
        
    }
}



/*
 * Helper Function Definitions
*/

// void init_gpio_extension() {}