#include <stdio.h>
#include "pico/stdlib.h"

#define GPIO_WATCH_PIN 16
#define LED_PIN 15

// Variable to count how many times the button has been pressed
volatile int button_press_count = 0;

// Gpio callback function
void gpio_callback(uint gpio, uint32_t events){
    // Wait 10ms to debounce the button
    sleep_ms(100);
    // Check if the button is still pressed
    if (gpio_get(GPIO_WATCH_PIN) == 1) {
        // Increment the button press count
        button_press_count++;

        // Flip the LED state
        gpio_put(LED_PIN, !gpio_get(LED_PIN));

        // Format the button press count as a string
        char message[50];
        snprintf(message, sizeof(message), "Button pressed %d times\n", button_press_count);

        // Send the button press count over usb
        printf("%s", message);
    }

}


int main()
{
    stdio_init_all();

    // Initialize gpio and set the irq
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Turn on the LED

    gpio_init(GPIO_WATCH_PIN);
    gpio_set_dir(GPIO_WATCH_PIN, GPIO_IN);
    gpio_pull_up(GPIO_WATCH_PIN); // Configure the pin to use the pull-up resistor
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (true);
}
