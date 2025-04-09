#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LED_PIN 15
#define GPIO_WATCH_PIN 16

// Variable whether to prompt the user
volatile bool prompt_user = false;


// Gpio callback function
void gpio_callback(uint gpio, uint32_t events){
    // Wait 10ms to debounce the button
    sleep_ms(100);
    // Check if the button is still pressed
    if (gpio_get(GPIO_WATCH_PIN) == 0) {
        // Clear the interrupt flag
        gpio_acknowledge_irq(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL);
        return;
        
    }

    // Flip the LED state
    gpio_put(LED_PIN, !gpio_get(LED_PIN));

    // Flip the prompt_user variable
    prompt_user = !prompt_user;

    // Clear the intterupt flag
    gpio_acknowledge_irq(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL);
}


int main() {
    stdio_init_all();
    // Wait for USB connection
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    // Initialize led pin and turn it on
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Turn on the LED

    // Initialize the ADC
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    // Initialize the button pin and set the button irq
    gpio_init(GPIO_WATCH_PIN);
    gpio_set_dir(GPIO_WATCH_PIN, GPIO_IN);
    gpio_pull_up(GPIO_WATCH_PIN); // Configure the pin to use the pull-up resistor
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);


    while (1) {
        // Check if the prompt_user variable is true
        if (prompt_user) {
            // Prompt the user for the number of analog samples to take
            int num_samples = 0;
            printf("Enter number of analog sample to collect (1-100): \r\n");
            scanf("%d", &num_samples);

            // Check if the number of samples is within the range
            if (num_samples > 100) {
                num_samples = 100;
            } else if (num_samples < 1) {
                num_samples = 1;
            }

            // Collect the samples
            int samples[num_samples];
            for (int i = 0; i < num_samples; i++) {
                // Read the ADC value
                uint16_t adc_value = adc_read();
                // Store the sample in the array
                samples[i] = adc_value;
                sleep_ms(10); // Wait 10ms between samples
            }

            // Print the samples
            for (int i = 0; i < num_samples; i++) {
                printf("Sample %d: %1.2f V, ", i + 1, samples[i] * 3.3f / 4096.0f);
                printf("\r\n");
            }

        
        }
    
        
    }
}