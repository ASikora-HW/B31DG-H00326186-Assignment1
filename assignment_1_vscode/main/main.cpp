#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "App"

#define SIGNAL_A_PIN GPIO_NUM_23
#define SIGNAL_B_PIN GPIO_NUM_16
#define BUTTON_1_PIN GPIO_NUM_14
#define BUTTON_2_PIN GPIO_NUM_15

// Debug mode select
#define DEBUG_MODE

#ifdef DEBUG_MODE
#define TIME_SLOW_FACTOR 1000
#else
#define TIME_SLOW_FACTOR 1
#endif

#define PULSE_DURATION ((8 * 100) * TIME_SLOW_FACTOR)         // microseconds
#define BETWEEN_PULSE_OFF_TIME ((9 * 100) * TIME_SLOW_FACTOR) // microseconds
#define PULSE_COUNT (11 + 4)
#define IDLE_TIME ((12 * 500) * TIME_SLOW_FACTOR) // microseconds
#define SYNC_ON_TIME (50 * TIME_SLOW_FACTOR)      // microseconds

// Signal variables
bool outputEnabled = false;
bool alternateMode = false;

// Button state tracking
bool lastEnableState = false;
bool lastSelectState = false;

void buttonDebounce()
{
    bool currentEnableState = gpio_get_level(BUTTON_1_PIN);
    bool currentSelectState = gpio_get_level(BUTTON_2_PIN);

    // Check for state change with simple debounce logic
    if (currentEnableState != lastEnableState && currentEnableState == 1)
    {
        outputEnabled = !outputEnabled; // Toggle output enable state
        printf(outputEnabled ? "Output Enabled\n" : "Output Disabled\n");
    }

    if (currentSelectState != lastSelectState && currentSelectState == 1)
    {
        alternateMode = !alternateMode; // Toggle output mode/state
        printf(alternateMode ? "Mode 2 Selected\n" : "Default Mode Selected\n");
    }

    // Update last states
    lastEnableState = currentEnableState;
    lastSelectState = currentSelectState;
}

void outputEnable()
{
    // Start SYNC signal
    gpio_set_level(SIGNAL_B_PIN, 1);
    esp_rom_delay_us(SYNC_ON_TIME);
    gpio_set_level(SIGNAL_B_PIN, 0);

    if (!alternateMode)
    { // Default signal generation
        for (int i = 0; i < PULSE_COUNT; i++)
        {
            // Generate DATA pulse
            gpio_set_level(SIGNAL_A_PIN, 1);
            esp_rom_delay_us(PULSE_DURATION + (i * (50 * TIME_SLOW_FACTOR))); // increase pulse duration for each pulse after 1st
            gpio_set_level(SIGNAL_A_PIN, 0);
            esp_rom_delay_us(BETWEEN_PULSE_OFF_TIME);
        }
    }
    else if (alternateMode)
    { // Alternate signal generation
        // changed loop to decrement i to have durations swap for alternate mode
        for (int i = PULSE_COUNT; i > 0; i--)
        {
            gpio_set_level(SIGNAL_A_PIN, 1);
            esp_rom_delay_us(PULSE_DURATION + (i * (50 * TIME_SLOW_FACTOR))); // decrease pulse duration for each pulse after 1st
            gpio_set_level(SIGNAL_A_PIN, 0);
            esp_rom_delay_us(BETWEEN_PULSE_OFF_TIME);
        }
    }

    // Idle time before next SYNC pulse
    esp_rom_delay_us(IDLE_TIME);
}

extern "C"
{
    void app_main(void)
    {
        gpio_set_direction(SIGNAL_A_PIN, GPIO_MODE_OUTPUT);
        gpio_set_direction(SIGNAL_B_PIN, GPIO_MODE_OUTPUT);
        gpio_set_direction(BUTTON_1_PIN, GPIO_MODE_INPUT);
        gpio_set_pull_mode(BUTTON_1_PIN, GPIO_PULLDOWN_ONLY);
        gpio_set_direction(BUTTON_2_PIN, GPIO_MODE_INPUT);
        gpio_set_pull_mode(BUTTON_2_PIN, GPIO_PULLDOWN_ONLY);

        while (1)
        {
            buttonDebounce();
            if (outputEnabled)
            {
                outputEnable();
            }
        }
    }
}