/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define CORE0_READY 10
#define CORE1_READY 11

#define FLAG_READ_ADC 1
#define FLAG_TURN_ON_LED 2
#define FLAG_TURN_OFF_LED 3

#define FLAG_CORE1_ERR 20

#define LED_PIN 15

volatile uint16_t adc_val;

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

    while(!multicore_fifo_rvalid()){;}
    uint32_t g = multicore_fifo_pop_blocking();

    if (g != CORE0_READY)
        printf("Hmm, that's not right on core 1!\n");
    else
        printf("Its all gone well on core 1!\n");

    while (1){

        while(!multicore_fifo_rvalid()){;}
        uint32_t msg_core0 = multicore_fifo_pop_blocking();

        switch (msg_core0) {
            case FLAG_READ_ADC:
                adc_val = adc_read();
                multicore_fifo_push_blocking(CORE1_READY);
                break;

            case FLAG_TURN_ON_LED:
                gpio_put(LED_PIN, 1);
                multicore_fifo_push_blocking(CORE1_READY);
                break;

            case FLAG_TURN_OFF_LED:
                gpio_put(LED_PIN, 0); // Correctly turn off the LED
                multicore_fifo_push_blocking(CORE1_READY);
                break;

            default:
                printf("Unknown command received on core 1: %u\r\n", msg_core0);
                multicore_fifo_push_blocking(CORE1_READY); // Acknowledge unknown command
                break;
        }
        
    }
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    /// \tag::setup_multicore[]
    multicore_launch_core1(core1_entry);
    sleep_ms(1);

    // Wait for it to start up
    multicore_fifo_push_blocking(CORE0_READY);
    while(!multicore_fifo_rvalid()){;}
    uint32_t g = multicore_fifo_pop_blocking();

    if (g != CORE1_READY)
        printf("Hmm, that's not right on core 0!\n");
    else {
        multicore_fifo_push_blocking(CORE0_READY);
        printf("It's all gone well on core 0!\n");
    }

    uint32_t usr_msg;

    while (true){
        printf("Send a command:\r\n1: Read Voltage\r\n2: turn on LED\r\n3: turn off LED\r\n");
        scanf("%d", &usr_msg);
        printf("Received msg %d\r\n", usr_msg);

        switch (usr_msg) {
            case 1:
                multicore_fifo_push_blocking(FLAG_READ_ADC);
                break;

            case 2:
                multicore_fifo_push_blocking(FLAG_TURN_ON_LED);
                break;

            case 3:
                multicore_fifo_push_blocking(FLAG_TURN_OFF_LED);
                break;

            // Remove default case to avoid sending invalid commands
        }
        

        while(!multicore_fifo_rvalid()){;}
        g = multicore_fifo_pop_blocking();
        float adc_val_float = adc_val * 3.3f / 4095.0f;
        if (usr_msg == FLAG_READ_ADC && g == CORE1_READY){
            printf("ADC reading: %1.2f V\r\n", adc_val_float); // Remove '&'
        }

        if (g == FLAG_CORE1_ERR){
            printf("CORE 1 ERROR");
            while (1) {;}
        }

    }

    /// \end::setup_multicore[]
}

