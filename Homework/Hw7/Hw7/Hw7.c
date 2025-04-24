#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
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
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Set up the LED display driver
    ssd1306_setup();

    unsigned char str[20];
    sprintf(str, "Hello World!");
    ssd1306_print_string(10, 16, str);

    while (true) {

        

        
        // Flip the hearbeat LED at 1Hz
        if (to_ms_since_boot(get_absolute_time()) - start_time > 1000) { 
            start_time = to_ms_since_boot(get_absolute_time()); 
            gpio_put(25, !gpio_get(25)); 
        }

    }
}




/*
 * Helper function defintions
 */

 void ssd1306_print_string(unsigned char x, unsigned char y, unsigned char *string){
    unsigned char current_byte;
    unsigned char i = 0;

    while (string[i] != '\0') {
        current_byte = string[i];

        ssd1306_draw_ASCII(x, y, current_byte - 0x20);
        printf("ASCII: %d\n", current_byte);
        x += 7; // Move to the next character position
        i++;
        sleep_ms(100); // Add a delay between characters
    }
}
 
 void ssd1306_draw_ASCII(unsigned char x, unsigned char y, unsigned char character) {
    unsigned char current_byte;

    for (int i = 0; i < 5; i++) {
        current_byte = ASCII[character][i];

        ssd1306_update_vertical_byte(x + i, y, current_byte);
    }
    ssd1306_update();
}


void ssd1306_update_vertical_byte(unsigned char x, unsigned char y, unsigned char byte){
    if (0b1 & byte) {
        ssd1306_drawPixel(x, y, 1);
    } else {
        ssd1306_drawPixel(x, y, 0);
    }

    if (0b1 << 1 & byte) {
        ssd1306_drawPixel(x, y + 1, 1);
    } else {
        ssd1306_drawPixel(x, y + 1, 0);
    }

    if (0b1 << 2 & byte) {
        ssd1306_drawPixel(x, y + 2, 1);
    } else {
        ssd1306_drawPixel(x, y + 2, 0);
    }

    if (0b1 << 3 & byte) {
        ssd1306_drawPixel(x, y + 3, 1);
    } else {
        ssd1306_drawPixel(x, y + 3, 0);
    }
    
    if (0b1 << 4 & byte) {
        ssd1306_drawPixel(x, y + 4, 1);
    } else {
        ssd1306_drawPixel(x, y + 4, 0);
    }

    if (0b1 << 5 & byte) {
        ssd1306_drawPixel(x, y + 5, 1);
    } else {
        ssd1306_drawPixel(x, y + 5, 0);
    }

    if (0b1 << 6 & byte) {
        ssd1306_drawPixel(x, y + 6, 1);
    } else {
        ssd1306_drawPixel(x, y + 6, 0);
    }

    if (0b1 << 7 & byte) {
        ssd1306_drawPixel(x, y + 7, 1);
    } else {
        ssd1306_drawPixel(x, y + 7, 0);
    }
}