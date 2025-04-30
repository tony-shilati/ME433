#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "ws2812.pio.h" 
#include "colors.h"
#include "math.h"
// Preprocessor directives related to the WS2812 configuration
#define IS_RGBW false
#define NUM_PIXELS 5
#define WS2812_PIN 13
#define SERVO_PIN 5 // the built in LED on the Pico

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

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN); // Get PWM slice number
    float div = 255; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 11765; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(SERVO_PIN, wrap * 0.025); // set the duty cycle to 50%

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

    wsColor colors[NUM_PIXELS];

    
    uint16_t led_counter = 0;


    while (true) { 
        // Update the position of the servo
        if (led_counter < 2048) {
            pwm_set_gpio_level(SERVO_PIN, wrap * (0.025 + led_counter / 2048.0f * 0.1)); // set the duty cycle to 50%
        } else {
            pwm_set_gpio_level(SERVO_PIN, wrap * (0.025 + (4095 - led_counter) / 2048.0f * 0.1)); // set the duty cycle to 50%
        }
        

        // Update the colors of the LEDs
        colors[0] = HSBtoRGB((0.5 + 0.5*sin(led_counter / 2047.0f * M_PI)) * 360, 1.0, 1.0);
        colors[1] = HSBtoRGB((0.5 + 0.5*sin(led_counter / 2047.0f * M_PI + M_PI/5)) * 360, 1.0, 1.0);
        colors[2] = HSBtoRGB((0.5 + 0.5*sin(led_counter / 2047.0f * M_PI + 2*M_PI/5)) * 360, 1.0, 1.0);
        colors[3] = HSBtoRGB((0.5 + 0.5*sin(led_counter / 2047.0f * M_PI + 3*M_PI/5)) * 360, 1.0, 1.0);
        colors[4] = HSBtoRGB((0.5 + 0.5*sin(led_counter / 2047.0f * M_PI + 4*M_PI/5)) * 360, 1.0, 1.0);

        for(int i=0;i<NUM_PIXELS;i++){
            put_pixel(pio, sm, urgb_u32(colors[i].r, colors[i].g, colors[i].b)); // assuming you've made arrays of colors to send
        }
        sleep_ms(1); // wait at least the reset time

        led_counter++;
        if (led_counter > 4095) {
            led_counter = 0;
        }
    }
}
