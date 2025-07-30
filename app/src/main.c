#include <stdint.h>
#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/app_version.h>
#include <zephyr/version.h>
#include <zephyr/autoconf.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/cellular.h>
#include <zephyr/drivers/uart.h>

#include "main.h"
#include "debug.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#if DT_NODE_EXISTS(DT_NODELABEL(io_expander))
static const struct i2c_dt_spec io_expander = I2C_DT_SPEC_GET(DT_NODELABEL(io_expander));
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(fuel_gauge))
static const struct device *const fuel_gauge = DEVICE_DT_GET_ANY(maxim_max17048);
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_pwr_en))
static const struct gpio_dt_spec mdm_pwr_en =
	GPIO_DT_SPEC_GET_OR(DT_NODELABEL(mdm_pwr_en), gpios, {0});
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_reset))
static const struct gpio_dt_spec mdm_reset =
	GPIO_DT_SPEC_GET_OR(DT_NODELABEL(mdm_reset), gpios, {0});
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_pwr_on))
static const struct gpio_dt_spec mdm_pwr_on =
	GPIO_DT_SPEC_GET_OR(DT_NODELABEL(mdm_pwr_on), gpios, {0});
#endif

#if DT_NODE_EXISTS(DT_CHOSEN(uart_modem))
static const struct device *const mdm_uart = DEVICE_DT_GET(DT_CHOSEN(uart_modem));
#endif

#define MCP9808 DT_NODELABEL(mcp9808)
static const struct device *const mcp9808 = DEVICE_DT_GET(MCP9808);

#define SPI_FLASH_TEST_REGION_OFFSET 0xff000
#define SPI_FLASH_SECTOR_SIZE        4096

#define NORFLASH DT_NODELABEL(norflash)
static const struct device *const norflash = DEVICE_DT_GET(NORFLASH);

const uint8_t erased[] = {0xff, 0xff, 0xff, 0xff};

void single_sector_test(const struct device *flash_dev)
{
	const uint8_t expected[] = {0x55, 0xaa, 0x66, 0x99};
	const size_t len = sizeof(expected);
	uint8_t buf[sizeof(expected)];
	int rc;

	LOG_INF("Perform test on single sector");
	/* Write protection needs to be disabled before each write or
	 * erase, since the flash component turns on write protection
	 * automatically after completion of write and erase
	 * operations.
	 */
	LOG_INF("Test 1: Flash erase");

	/* Full flash erase if SPI_FLASH_TEST_REGION_OFFSET = 0 and
	 * SPI_FLASH_SECTOR_SIZE = flash size
	 */
	rc = flash_erase(flash_dev, SPI_FLASH_TEST_REGION_OFFSET, SPI_FLASH_SECTOR_SIZE);
	if (rc != 0) {
		LOG_INF("Flash erase failed! %d", rc);
	} else {
		/* Check erased pattern */
		memset(buf, 0, len);
		rc = flash_read(flash_dev, SPI_FLASH_TEST_REGION_OFFSET, buf, len);
		if (rc != 0) {
			LOG_INF("Flash read failed! %d", rc);
			return;
		}
		if (memcmp(erased, buf, len) != 0) {
			LOG_INF("Flash erase failed at offset 0x%x got 0x%x",
				SPI_FLASH_TEST_REGION_OFFSET, *(uint32_t *)buf);
			return;
		}
		LOG_INF("Flash erase succeeded!");
	}
	LOG_INF("Test 2: Flash write");

	LOG_INF("Attempting to write %zu bytes", len);
	rc = flash_write(flash_dev, SPI_FLASH_TEST_REGION_OFFSET, expected, len);
	if (rc != 0) {
		LOG_INF("Flash write failed! %d", rc);
		return;
	}

	memset(buf, 0, len);
	rc = flash_read(flash_dev, SPI_FLASH_TEST_REGION_OFFSET, buf, len);
	if (rc != 0) {
		LOG_INF("Flash read failed! %d", rc);
		return;
	}

	if (memcmp(expected, buf, len) == 0) {
		LOG_INF("Data read matches data written. Good!!");
	} else {
		const uint8_t *wp = expected;
		const uint8_t *rp = buf;
		const uint8_t *rpe = rp + len;

		LOG_INF("Data read does not match data written!!");
		while (rp < rpe) {
			LOG_INF("%08x wrote %02x read %02x %s",
				(uint32_t)(SPI_FLASH_TEST_REGION_OFFSET + (rp - buf)), *wp, *rp,
				(*rp == *wp) ? "match" : "MISMATCH");
			++rp;
			++wp;
		}
	}
}

#if DT_NODE_EXISTS(DT_NODELABEL(io_expander))
static inline bool init_io_expander(void)
{
	if (!device_is_ready(io_expander.bus)) {
		LOG_ERR("IO expander bus %s is not ready", io_expander.bus->name);
		return false;
	}
	return true;
}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(fuel_gauge))
static inline bool init_fuel_gauge(void)
{
	if (fuel_gauge == NULL) {
		LOG_ERR("Fuel gauge not found");
		return false;
	}

	if (!device_is_ready(fuel_gauge)) {
		LOG_ERR("Fuel gauge not ready");
		return false;
	}

	return true;
}
#endif

static bool init_modem_pins(void)
{
	int status = 0;

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_pwr_en))
	if (!gpio_is_ready_dt(&mdm_pwr_en)) {
		LOG_ERR("MDM_PWR_EN not ready");
		return false;
	}
	status = gpio_pin_configure_dt(&mdm_pwr_en, GPIO_OUTPUT_LOW);
	if (status < 0) {
		LOG_ERR("Could not configure MDM_PWR_EN");
		return false;
	}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_reset))
	if (!gpio_is_ready_dt(&mdm_reset)) {
		LOG_ERR("MDM_RESET not ready");
		return false;
	}
	status = gpio_pin_configure_dt(&mdm_reset, GPIO_OUTPUT_LOW);
	if (status < 0) {
		LOG_ERR("Could not configure MDM_RESET");
		return false;
	}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_pwr_on))
	if (!gpio_is_ready_dt(&mdm_pwr_on)) {
		LOG_ERR("MDM_PWR_ON not ready");
		return false;
	}
	status = gpio_pin_configure_dt(&mdm_pwr_on, GPIO_OUTPUT_LOW);
	if (status < 0) {
		LOG_ERR("Could not configure MDM_PWR_ON");
		return false;
	}
#endif

	return true;
}

int main(void)
{
	if (usb_enable(NULL)) {
		return 0;
	}

	printk("Iida devkit application: %s %s\n", APP_VERSION_STRING,
	       STRINGIFY(APP_BUILD_VERSION));
	printk("Zephyr: %s\n", STRINGIFY(BUILD_VERSION));
	printk("Board: %s\n", CONFIG_BOARD_TARGET);
	printk("Built on %s @ %s\n", __DATE__, __TIME__);

	int ret;
	bool mcp9808_okay = true;
	bool norflash_okay = true;
	uint32_t counter = 0UL;

	if (mcp9808 == NULL) {
		LOG_ERR("MCP9808 not found");
		mcp9808_okay = false;
	} else {
		if (!device_is_ready(mcp9808)) {
			LOG_ERR("MCP9808 not ready");
			mcp9808_okay = false;
		}
	}

	if (norflash == NULL) {
		LOG_ERR("NOR flash not found");
		norflash_okay = false;
	} else {
		if (!device_is_ready(norflash)) {
			LOG_ERR("FLASH device not ready");
			norflash_okay = false;
		}
	}

#if DT_NODE_EXISTS(DT_CHOSEN(uart_modem))
	bool mdm_uart_okay = true;
	if (mdm_uart == NULL) {
		LOG_ERR("MDM UART not found");
		mdm_uart_okay = false;
	} else {
		if (!device_is_ready(mdm_uart)) {
			LOG_ERR("MDM UART not ready");
			mdm_uart_okay = false;
		}
	}
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(fuel_gauge))
	bool fuel_gauge_okay = init_fuel_gauge();
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(io_expander))
	bool io_expander_okay = true;
	io_expander_okay = init_io_expander();
	if (io_expander_okay) {
		debug_init();
	}
#else
	debug_init();
#endif

#ifdef SELF_TEST
	if (norflash_okay) {
		single_sector_test(norflash);
	}
#endif

	init_modem_pins();

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_reset))
	(void)gpio_pin_set_dt(&mdm_reset, 0);
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_pwr_on))
	(void)gpio_pin_set_dt(&mdm_pwr_on, 0);
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_pwr_en))
	(void)gpio_pin_set_dt(&mdm_pwr_en, 1);
#endif

#if DT_NODE_EXISTS(DT_NODELABEL(mdm_pwr_on))
	k_busy_wait(1000 * 1000);
	(void)gpio_pin_set_dt(&mdm_pwr_on, 1);
	k_busy_wait(250 * 1000);
	(void)gpio_pin_set_dt(&mdm_pwr_on, 0);
#endif

	while (1) {
		if (mcp9808_okay) {
			ret = sensor_sample_fetch(mcp9808);
			if (ret != 0) {
				LOG_ERR("Could not sample TEMP SENS: %d\n", ret);
			} else {
				struct sensor_value temp_val;
				ret = sensor_channel_get(mcp9808, SENSOR_CHAN_AMBIENT_TEMP,
							 &temp_val);
				if (ret != 0) {
					LOG_ERR("Could not get ambient temperature form TEMP SENS: "
						"%d\n",
						ret);
				} else {
					double temp = sensor_value_to_double(&temp_val);
					LOG_INF("TEMP SENS: %f C", temp);
				}
			}
		}

#if DT_NODE_EXISTS(DT_NODELABEL(fuel_gauge))
		if (fuel_gauge_okay) {
			union fuel_gauge_prop_val batt_val = {0};
			ret = fuel_gauge_get_prop(fuel_gauge, FUEL_GAUGE_RELATIVE_STATE_OF_CHARGE,
						  &batt_val);
			if (ret < 0) {
				LOG_ERR("Could not get battery SoC");
			} else {
				LOG_INF("BATT SoC: %u %%", batt_val.relative_state_of_charge);
			}

			union fuel_gauge_prop_val volt_val = {0};
			ret = fuel_gauge_get_prop(fuel_gauge, FUEL_GAUGE_VOLTAGE, &volt_val);
			if (ret < 0) {
				LOG_ERR("Could not get battery voltage");
			} else {
				double voltage = (double)(volt_val.voltage) / 1000 / 1000;
				LOG_INF("BATT voltage: %0.2f %%", voltage);
			}
		}
#endif

		debug_gpio_toggle();

		for (int i = 0; i < 3; i++) {
			if (i == 0) {
				debug_led_toggle(DBG_LED_A);
			} else if (i == 1) {
				debug_led_toggle(DBG_LED_B);
			} else if (i == 2) {
				debug_led_toggle(DBG_LED_C);
			}
			k_sleep(K_MSEC(100));
		}

#if DT_NODE_EXISTS(DT_CHOSEN(uart_modem))
		if (mdm_uart_okay) {
			static const char *at_msg = "AT\r\n";
			for (int i = 0; i < strlen(at_msg); i++) {
				uart_poll_out(mdm_uart, at_msg[i]);
			}
			k_sleep(K_MSEC(1000));
		}
#endif

		LOG_INF("Counter: %" PRIu32, counter++);
	}
}
