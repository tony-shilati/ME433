#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// SPI chip select control
static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}

void send_signal(uint16_t signa, char channel);


int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 20*1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    uint16_t signal = 1;
    while (true) {
        send_signal(signal, 'a');
        signal+=16;
        if (signal > 1024) {
            signal = 0b1;
        }
        sleep_ms(5);
    }
}


void send_signal(uint16_t signal, char channel){

    // Squash the signal to 10 bits
    if (signal > 1024) {
        signal = 1024;
    }

    // Shift the signal to the left by 2 bits
    signal = signal << 2;

    // Set the channel bit
    uint16_t channel_bit = 0;
    if (channel == 'a'){
        channel_bit = 0 << 15;
    } else if (channel == 'b'){
        channel_bit = 1 << 15;
    }

    // Set the config bits
    uint16_t config_bits = 0b111 << 12; // 3 bits for the config

    uint16_t spi_msg = 0;
    // Combine the signal, channel, and config bits
    spi_msg |= signal;
    spi_msg |= channel_bit;
    spi_msg |= config_bits;

    // Pull the chip slect pin low
    cs_select();
    // Send the signal

    uint8_t spi_msg_arry[2];
    spi_msg_arry[0] = (spi_msg >> 8) & 0xFF; // MSB
    spi_msg_arry[1] = spi_msg & 0xFF; // LSB

    spi_write_blocking(spi_default, spi_msg_arry, 2);

    // Pull the chip select pin high
    cs_deselect();

}


