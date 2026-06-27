#include <zephyr/kernel.h>
#include <zephyr/app_version.h>
#include <zephyr/version.h>
#include <zephyr/autoconf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "main.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET(DT_NODELABEL(led_a), gpios);
static const struct gpio_dt_spec ledB = GPIO_DT_SPEC_GET(DT_NODELABEL(led_b), gpios);
static const struct gpio_dt_spec ledC = GPIO_DT_SPEC_GET(DT_NODELABEL(led_c), gpios);

int main(void)
{
	LOG_INF("Iida devkit application: %s %s\n", APP_VERSION_STRING,
		STRINGIFY(APP_BUILD_VERSION));
	LOG_INF("Zephyr: %s\n", STRINGIFY(BUILD_VERSION));
	LOG_INF("Board: %s\n", CONFIG_BOARD_TARGET);
	LOG_INF("Built on %s @ %s\n", __DATE__, __TIME__);

	int err = 0;
	err = gpio_pin_configure_dt(&ledA, GPIO_OUTPUT_INACTIVE);
	if (err) {
		LOG_ERR("Error configuring LED A: %d", err);
		return err;
	}

	err = gpio_pin_configure_dt(&ledB, GPIO_OUTPUT_INACTIVE);
	if (err) {
		LOG_ERR("Error configuring LED B: %d", err);
		return err;
	}

	err = gpio_pin_configure_dt(&ledC, GPIO_OUTPUT_INACTIVE);
	if (err) {
		LOG_ERR("Error configuring LED C: %d", err);
		return err;
	}

	while (1) {
		for (int i = 0; i < 3; i++) {
			gpio_pin_toggle_dt(&ledA);
			k_msleep(100);
			gpio_pin_toggle_dt(&ledB);
			k_msleep(100);
			gpio_pin_toggle_dt(&ledC);
			k_msleep(100);
		}
	}
}
