#include "debug.h"

#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(debug, CONFIG_APP_LOG_LEVEL);

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_gpio))
static const struct gpio_dt_spec dbg_gpio = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(dbg_gpio), gpios, {0});
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_led_a))
static const struct gpio_dt_spec dbg_led_a =
	GPIO_DT_SPEC_GET_OR(DT_NODELABEL(dbg_led_a), gpios, {0});
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_led_b))
static const struct gpio_dt_spec dbg_led_b =
	GPIO_DT_SPEC_GET_OR(DT_NODELABEL(dbg_led_b), gpios, {0});
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_led_c))
static const struct gpio_dt_spec dbg_led_c =
	GPIO_DT_SPEC_GET_OR(DT_NODELABEL(dbg_led_c), gpios, {0});
#endif

static bool debug_gpio_okay = true;
static bool debug_led_a_okay = true;
static bool debug_led_b_okay = true;
static bool debug_led_c_okay = true;

void debug_init(void)
{
	int status = 0;

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_gpio))
	if (!gpio_is_ready_dt(&dbg_gpio)) {
		LOG_ERR("DBG GPIO not ready");
		debug_gpio_okay = false;
	}
	if (debug_gpio_okay) {
		status = gpio_pin_configure_dt(&dbg_gpio, GPIO_OUTPUT_INACTIVE);
		if (status < 0) {
			LOG_ERR("Could not configure DBG GPIO");
			debug_gpio_okay = false;
		}
	}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_led_a))
	status = gpio_pin_configure_dt(&dbg_led_a, GPIO_OUTPUT_INACTIVE);
	if (status < 0) {
		debug_led_a_okay = false;
	}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_led_b))
	status = gpio_pin_configure_dt(&dbg_led_b, GPIO_OUTPUT_INACTIVE);
	if (status < 0) {
		debug_led_b_okay = false;
	}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(dbg_led_c))
	status = gpio_pin_configure_dt(&dbg_led_c, GPIO_OUTPUT_INACTIVE);
	if (status < 0) {
		debug_led_c_okay = false;
	}
#endif
}

void debug_gpio_set(bool state)
{
	if (!debug_gpio_okay) {
		return;
	}
	(void)gpio_pin_set_dt(&dbg_gpio, state ? 1 : 0);
}

void debug_gpio_toggle()
{
	if (!debug_gpio_okay) {
		return;
	}
	(void)gpio_pin_toggle_dt(&dbg_gpio);
}

void debug_led_set(enum dbg_led led, bool state)
{
	switch (led) {
	case DBG_LED_A:
		if (!debug_led_a_okay) {
			break;
		}
		(void)gpio_pin_set_dt(&dbg_led_a, state ? 1 : 0);
		break;
	case DBG_LED_B:
		if (!debug_led_b_okay) {
			break;
		}
		(void)gpio_pin_set_dt(&dbg_led_b, state ? 1 : 0);
		break;
	case DBG_LED_C:
		if (!debug_led_c_okay) {
			break;
		}
		(void)gpio_pin_set_dt(&dbg_led_c, state ? 1 : 0);
		break;
	default:
		LOG_ERR("Unknown LED");
		break;
	}
}

void debug_led_toggle(enum dbg_led led)
{
	switch (led) {
	case DBG_LED_A:
		if (!debug_led_a_okay) {
			break;
		}
		(void)gpio_pin_toggle_dt(&dbg_led_a);
		break;
	case DBG_LED_B:
		if (!debug_led_b_okay) {
			break;
		}
		(void)gpio_pin_toggle_dt(&dbg_led_b);
		break;
	case DBG_LED_C:
		if (!debug_led_c_okay) {
			break;
		}
		(void)gpio_pin_toggle_dt(&dbg_led_c);
		break;
	default:
		LOG_ERR("Unknown LED");
		break;
	}
}
