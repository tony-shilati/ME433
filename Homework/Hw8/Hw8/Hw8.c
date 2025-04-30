#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h" 

// Preprocessor directives related to the WS2812 configuration
#define IS_RGBW false
#define NUM_PIXELS 4
#define WS2812_PIN 13

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 16) |
            ((uint32_t) (g) << 8) |
            (uint32_t) (b);
}



int main()
{
    stdio_init_all();

    // todo get free sm
    PIO pio;
    uint sm;
    uint offset;

    // This will find a free pio and state machine for our program and load it for us
    // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
    // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);


    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    int r[NUM_PIXELS] = {255, 0, 0, 255};
    int g[NUM_PIXELS] = {0, 255, 0, 255};
    int b[NUM_PIXELS] = {0, 0, 255, 255};

    while (true) {

        for(int i=0;i<NUM_PIXELS;i++){
            put_pixel(pio, sm, urgb_u32(r[i], g[i], b[i])); // assuming you've made arrays of colors to send
        }
        sleep_ms(100); // wait at least the reset time

    }
}
