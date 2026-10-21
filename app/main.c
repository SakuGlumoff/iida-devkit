#include <zephyr/kernel.h>
#include <zephyr/app_version.h>
#include <zephyr/version.h>
#include <zephyr/autoconf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/usb/usbd.h>

#include "main.h"
#include "usbd_init.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_APP_LOG_LEVEL);

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET(DT_NODELABEL(led_a), gpios);

static struct usbd_context *usbd_ctx;

static void usbd_cb(struct usbd_context *const ctx, const struct usbd_msg *msg)
{
	LOG_DBG("USBD message: %s", usbd_msg_type_string(msg->type));

	if (usbd_can_detect_vbus(ctx)) {
		if (msg->type == USBD_MSG_VBUS_READY) {
			if (usbd_enable(ctx)) {
				LOG_ERR("Failed to enable device support");
			}
		}

		if (msg->type == USBD_MSG_VBUS_REMOVED) {
			if (usbd_disable(ctx)) {
				LOG_ERR("Failed to disable device support");
			}
		}
	}
}

static int enable_usb_device_next(void)
{
	int err;

	usbd_ctx = usbd_init_device(usbd_cb);
	if (usbd_ctx == NULL) {
		LOG_ERR("Failed to initialize USB device");
		return -ENODEV;
	}

	if (!usbd_can_detect_vbus(usbd_ctx)) {
		err = usbd_enable(usbd_ctx);
		if (err) {
			LOG_ERR("Failed to enable device support");
			return err;
		}
	}

	LOG_DBG("USB device support enabled");

	return 0;
}

int main(void)
{
	LOG_INF("Iida devkit application: %s %s", APP_VERSION_STRING, STRINGIFY(APP_BUILD_VERSION));
	LOG_INF("Zephyr: %s", STRINGIFY(BUILD_VERSION));
	LOG_INF("Board: %s", CONFIG_BOARD_TARGET);
	LOG_INF("Built on %s @ %s", __DATE__, __TIME__);

	int err = 0;
	err = gpio_pin_configure_dt(&ledA, GPIO_OUTPUT_ACTIVE);
	if (err) {
		LOG_ERR("Error configuring LED A: %d", err);
		return err;
	}

	err = enable_usb_device_next();
	if (err != 0) {
		LOG_ERR("Could not enable USBD: %d", err);
		return err;
	}

	while (1) {
		err = gpio_pin_toggle_dt(&ledA);
		if (err) {
			LOG_ERR("Error toggling LED A: %d", err);
		}
		k_msleep(1000);
	}
}
