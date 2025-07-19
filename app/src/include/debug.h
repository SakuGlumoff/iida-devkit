#pragma once

#include <stdbool.h>

enum dbg_led {
	DBG_LED_A,
	DBG_LED_B,
	DBG_LED_C
};

/**
 * @brief Initialize the debug GPIO pin and the debug LEDs.
 */
void debug_init(void);

/**
 * @brief Set the debug GPIO pin to a specific state.
 *
 * @param[in] state The state to set the pin to. TRUE for ON, FALSE for OFF.
 */
void debug_gpio_set(bool state);

/**
 * @brief Toggle the debug GPIO pin state.
 */
void debug_gpio_toggle();

/**
 * @brief Set the debug LED to a specific state.
 *
 * @param[in] led   The debug LED to set.
 * @param[in] state The state to set the LED to. TRUE for ON, FALSE for OFF.
 */
void debug_led_set(enum dbg_led led, bool state);

/**
 * @brief Toggle the debug LED state.
 *
 * @param[in] led The debug LED to toggle.
 */
void debug_led_toggle(enum dbg_led led);
