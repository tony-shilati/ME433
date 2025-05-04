/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#define CORE0_READY 10
#define CORE1_READY 11

#define FLAG_READ_ADC 1
#define FLAG_TURN_ON_LED 2
#define FLAG_TURN_OFF_LED 3

#define LED_PIN 15

void core1_entry() {

    // Initialize LED pin
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Turn off the LED\

    // Initialize the ADC
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    multicore_fifo_push_blocking(CORE1_READY);
    uint32_t g = multicore_fifo_pop_blocking();

    if (g != CORE0_READY)
        printf("Hmm, that's not right on core 1!\n");
    else
        printf("Its all gone well on core 1!\n");

    while (1)
        tight_loop_contents();
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }


    printf("Send a command:\r\n1: Read Volatage\r\n2: turn on LED\r\n3: turn off LED\r\n");

    /// \tag::setup_multicore[]
    multicore_launch_core1(core1_entry);
    sleep_ms(1);

    // Wait for it to start up
    multicore_fifo_push_blocking(CORE0_READY);
    uint32_t g = multicore_fifo_pop_blocking();

    if (g != CORE1_READY)
        printf("Hmm, that's not right on core 0!\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("It's all gone well on core 0!\n");
    }

    while (true){

    }

    /// \end::setup_multicore[]
}
