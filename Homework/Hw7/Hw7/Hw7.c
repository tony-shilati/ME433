#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void ssd1306_draw_ASCII(unsigned char x, unsigned char y, unsigned char character);
void ssd1306_update_vertical_byte(unsigned char x, unsigned char y, unsigned char byte);
void ssd1306_print_string(unsigned char x, unsigned char y, unsigned char *string);



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

    // Set up the LED display driver
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    // Initialize the ADC
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    
    float frame_rate = 0.0; // Initialize frame rate variable
    uint64_t loop_time;
    while (true) {
        loop_time = to_ms_since_boot(get_absolute_time());

        // Print the ADC0 value on the display
        uint16_t adc_value = adc_read();
        float voltage = (adc_value / 4095.0) * 3.3; // Convert ADC value to voltage
        char buffer[20];
        sprintf(buffer, "ADC0: %1.2f V", voltage);
        ssd1306_print_string(0, 0, buffer);

        // Print the framerate of the display below the ADC value
        sprintf(buffer, " FPS: %1.2f Hz", frame_rate);
        ssd1306_print_string(0, 12, buffer); // Print the frame rate to the display

        // Flip the hearbeat LED at 1Hz
        if (to_ms_since_boot(get_absolute_time()) - start_time > 1000) { 
            start_time = to_ms_since_boot(get_absolute_time()); 
            gpio_put(25, !gpio_get(25)); 
        }

        // Calculate the frame rate
        frame_rate = 1000.0 / (to_ms_since_boot(get_absolute_time()) - loop_time); // Calculate frame rate
    }
}




/*
 * Helper function defintions
 */

 void ssd1306_print_string(unsigned char x, unsigned char y, unsigned char *string){
    unsigned char current_byte;
    unsigned char i = 0;
    unsigned char j = 0;

    while (string[i] != '\0') {
        current_byte = string[i];

        ssd1306_draw_ASCII(x, y, current_byte - 0x20);
        x += 7; // Move to the next character position

        if (x > 123) {
            x = 0; // Reset x position if it exceeds the screen width
            y += 8; // Move to the next line
        }

        if (y > 24) {
            y = 0; // Reset y position if it exceeds the screen height
        }

        i++;
    }
}
 
 void ssd1306_draw_ASCII(unsigned char x, unsigned char y, unsigned char character) {
    unsigned char current_byte;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            if (ASCII[character][i] & (1 << j)) {
                ssd1306_drawPixel(x + i, y + j, 1);
            } else {
                ssd1306_drawPixel(x + i, y + j, 0);
            }
        }
    }
    
    ssd1306_update();
}