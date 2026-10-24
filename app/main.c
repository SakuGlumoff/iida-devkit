#include <zephyr/kernel.h>
#include <zephyr/app_version.h>
#include <zephyr/version.h>
#include <zephyr/autoconf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/drivers/cellular.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/ppp.h>
#include <zephyr/pm/device.h>

#include "main.h"
#include "usbd_init.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_APP_LOG_LEVEL);

static const struct gpio_dt_spec ledA = GPIO_DT_SPEC_GET(DT_NODELABEL(led_a), gpios);

static const struct device *modem = DEVICE_DT_GET(DT_ALIAS(modem));

static struct usbd_context *usbd_ctx;

static struct net_mgmt_event_callback l4_cb;

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

static void modem_log_info(void)
{
	char buf[64];
	int16_t rssi;
	enum cellular_registration_status status;

	if (cellular_get_modem_info(modem, CELLULAR_MODEM_INFO_MODEL_ID, buf, sizeof(buf)) == 0) {
		LOG_INF("Modem model: %s", buf);
	}

	if (cellular_get_modem_info(modem, CELLULAR_MODEM_INFO_FW_VERSION, buf, sizeof(buf)) == 0) {
		LOG_INF("Modem firmware: %s", buf);
	}

	if (cellular_get_modem_info(modem, CELLULAR_MODEM_INFO_IMEI, buf, sizeof(buf)) == 0) {
		LOG_INF("Modem IMEI: %s", buf);
	}

	if (cellular_get_modem_info(modem, CELLULAR_MODEM_INFO_SIM_ICCID, buf, sizeof(buf)) == 0) {
		LOG_INF("SIM ICCID: %s", buf);
	}

	if (cellular_get_registration_status(modem, CELLULAR_ACCESS_TECHNOLOGY_E_UTRAN,
					     &status) == 0) {
		LOG_INF("Registration status: %d", status);
	}

	if (cellular_get_signal(modem, CELLULAR_SIGNAL_RSSI, &rssi) == 0) {
		LOG_INF("RSSI: %d dBm", rssi);
	}
}

static void l4_event_handler(struct net_mgmt_event_callback *cb, uint64_t event,
			     struct net_if *iface)
{
	struct net_in_addr *ipv4;
	char addr[NET_IPV4_ADDR_LEN];

	switch (event) {
	case NET_EVENT_L4_CONNECTED:
		ipv4 = net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED);
		if (ipv4 != NULL) {
			net_addr_ntop(NET_AF_INET, ipv4, addr, sizeof(addr));
			LOG_INF("Cellular network connected, IP: %s", addr);
		} else {
			LOG_INF("Cellular network connected");
		}
		modem_log_info();
		break;
	case NET_EVENT_L4_DISCONNECTED:
		LOG_WRN("Cellular network disconnected");
		break;
	default:
		break;
	}
}

static int enable_modem(void)
{
	struct net_if *ppp_iface;
	int err;

	if (!device_is_ready(modem)) {
		LOG_ERR("Modem device not ready");
		return -ENODEV;
	}

	ppp_iface = net_if_get_first_by_type(&NET_L2_GET_NAME(PPP));
	if (ppp_iface == NULL) {
		LOG_ERR("PPP network interface not found");
		return -ENODEV;
	}

	net_mgmt_init_event_callback(&l4_cb, l4_event_handler,
				     NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED);
	net_mgmt_add_event_callback(&l4_cb);

	LOG_INF("Powering on modem");
	err = pm_device_action_run(modem, PM_DEVICE_ACTION_RESUME);
	if (err != 0 && err != -EALREADY) {
		LOG_ERR("Failed to resume modem: %d", err);
		return err;
	}

	err = net_if_up(ppp_iface);
	if (err != 0 && err != -EALREADY) {
		LOG_ERR("Failed to bring up PPP interface: %d", err);
		return err;
	}

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

	err = enable_modem();
	if (err != 0) {
		LOG_ERR("Could not enable modem: %d", err);
	}

	while (1) {
		err = gpio_pin_toggle_dt(&ledA);
		if (err) {
			LOG_ERR("Error toggling LED A: %d", err);
		}
		k_msleep(1000);
	}
}
