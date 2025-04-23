#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void ssd1306_draw_line(unsigned char x, unsigned char color){
    for (int i = 0; i < 32; i++) {
        ssd1306_drawPixel(x, i, color);
    }
    ssd1306_update();
}



int main()
{
    stdio_init_all();

    // Initialize GPIO 25 for heartbeat LED
    gpio_init(25); // Initialize GPIO 15
    gpio_set_dir(25, GPIO_OUT); // Set GPIO 15 as output for heartbeat LED
    gpio_put(25, 1); // Turn on the LED
    uint64_t start_time = to_ms_since_boot(get_absolute_time()); // Get the current time

    // I2C Initialization. Using it at 400Khz.
    i2c_init(I2C_PORT, 1000*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Set up the LED display driver
    ssd1306_setup();

    unsigned char color = 1;
    unsigned char dir = 0;
    unsigned char counter = 1;
    while (true) {
        ssd1306_draw_line(counter, color);
        
        if (counter == 0 || counter == 127) {
            dir = !dir;
        }

        if (dir == 0) {
            ssd1306_draw_line(counter-1, !color);
            counter++;
        } else {
            ssd1306_draw_line(counter+1, !color);
            counter--;
        }
        

        // Flip the hearbeat LED at 1Hz
        if (to_ms_since_boot(get_absolute_time()) - start_time > 1000) { 
            start_time = to_ms_since_boot(get_absolute_time()); 
            gpio_put(25, !gpio_get(25)); 
        }
    }
}
