#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define ADDR 0b0100000 // I2C address of the GPIO extension board

void init_gpio_extension();
void external_LED_button();



int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    gpio_init(7); // Initialize GPIO 15
    gpio_set_dir(7, GPIO_OUT); // Set GPIO 15 as output for heartbeat LED
    gpio_put(7, 0); // Reset the external GPIO 
    sleep_ms(100); // Sleep for 1 second
    gpio_put(7, 1);

    gpio_init(25); // Initialize GPIO 15
    gpio_set_dir(25, GPIO_OUT); // Set GPIO 15 as output for heartbeat LED
    gpio_put(25, 1); // Turn on the LED

    init_gpio_extension(); // Initialize the GPIO extension board

    uint64_t start_time = to_ms_since_boot(get_absolute_time()); // Get the current time
    while (true) {

        external_LED_button(); // Check the button state and set the LED accordingly

        if (to_ms_since_boot(get_absolute_time()) - start_time > 500) { // If 0.5 second has passed
            start_time = to_ms_since_boot(get_absolute_time()); // Reset the timer
            gpio_put(25, !gpio_get(25)); // Toggle the LED
        }
    
        // printf("Hello World!\n"); // Print Hello World to the console
        // sleep_ms(500); // Sleep for 100ms
        
    }
}



/*
 * Helper Function Definitions
*/

void init_gpio_extension() {
    // Write the IODIR register to make GP0 and input and GP7 an output
    uint8_t buf[2];
    buf[0] = 0x00; // IODIR register
    buf[1] = 0b00000001; // Set all pins to output
    i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); // Write to the IODIR register

    // Set the gpio polarity
    buf[0] = 0x01; // IPOL register
    buf[1] = 0b00000000; // Set all pins to direct polarity
    i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); // Write to the GPINTEN register

    // Set GP7 to high
    buf[0] = 0x0A; // OLAT register
    buf[1] = 0b10000000; // Set GP7 to high
    i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); // Write to the IODIR register

    sleep_ms(500); // Sleep for 100ms

    // Set GP7 to low
    buf[0] = 0x0A; // OLAT register
    buf[1] = 0b00000000; // Set GP7 to high
    i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); // Write to the IODIR register
}


void external_LED_button(){

    uint8_t button_state;
    uint8_t gpio_addr = 0x09;

    // Get the button state
    i2c_write_blocking(I2C_PORT, ADDR, &gpio_addr, 1, true); // Read the GPIO state
    i2c_read_blocking(I2C_PORT, ADDR, &button_state, 1, false); // Read the GPIO state

    uint8_t buf[2];
    if (!(button_state & 0b00000001)) { // If the button is pressed
        // Set GP7 to high
        buf[0] = 0x0A; // OLAT register
        buf[1] = 0b10000000; // Set GP7 to high
        i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); // Write to the IODIR register
        
    } else {
        // Set GP7 to high
        buf[0] = 0x0A; // OLAT register
        buf[1] = 0b00000000; // Set GP7 to high
        i2c_write_blocking(I2C_PORT, ADDR, buf, 2, false); // Write to the IODIR register
        
    }
}


