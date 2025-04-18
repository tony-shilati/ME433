#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS1  17
#define PIN_CS2  20
#define PIN_SCK  18
#define PIN_MOSI 19

// SPI chip select functions
static inline void cs1_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS1, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs1_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS1, 1);
    asm volatile("nop \n nop \n nop");
}

static inline void cs2_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS2, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs2_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS2, 1);
    asm volatile("nop \n nop \n nop");
}

// Union definition
union FloatInt {
    float f;
    uint32_t i;
};


// Function prototypes
void send_dac_signal(uint16_t signa, char channel);
void spi_ram_init();
void write_ext_memory_seq(float *data, uint16_t address);
float read_ext_memory_seq(uint16_t address);


int main()
{
    stdio_init_all();

    // SPI initialization
    spi_init(SPI_PORT, 20*1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS1,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CS2,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // Drive cs pins high
    gpio_set_dir(PIN_CS1, GPIO_OUT);
    gpio_set_dir(PIN_CS2, GPIO_OUT);
    gpio_put(PIN_CS1, 1);
    gpio_put(PIN_CS2, 1);

    // Write the waveform to the RAM
    union FloatInt sin_point;
    uint16_t write_address;
    for (int i = 0; i < 1000; i++){
        // Generate point on a sine wave and write to external memory
        sin_point.f = (sin(i / 1000.0f * 2 * M_PI) + 1);
        write_address = 4 * i; // 4 bytes per float

        cs1_select();
        spi_write_blocking(SPI_PORT, (uint8_t *)&write_address, 2);
        spi_write_blocking(SPI_PORT, (uint8_t *)&sin_point.i, sizeof(sin_point));
        cs1_deselect();
        sleep_ms(1);
    }

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}



/*
 * Helper function definitions
 */

 void spi_ram_init(){
    // Create the config data
    uint8_t config_data[2];
    config_data[0] = 0b00000001; // Select SRAM read/write mode
    config_data[1] = 0b01000000; // Set SRAM read/write mode

    // Write the config data to the SRAM chip
    cs2_select();
    spi_write_blocking(SPI_PORT, config_data, 2);
    cs2_deselect();
    sleep_ms(10); // Wait for the chip to process the config data

 }


 void send_dac_signal(uint16_t signal, char channel){

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

    // Convert to array of 8 bit uints
    uint8_t spi_msg_arry[2];
    spi_msg_arry[0] = (spi_msg >> 8) & 0xFF; // MSB
    spi_msg_arry[1] = spi_msg & 0xFF; // LSB

    // Pull the chip slect pin low
    cs1_select();

    // Send the signal
    spi_write_blocking(spi_default, spi_msg_arry, 2);

    // Pull the chip select pin high
    cs1_deselect();

}