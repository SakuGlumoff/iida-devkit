#include <stdint.h>
#include <inttypes.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/app_version.h>
#include <zephyr/version.h>
#include <zephyr/autoconf.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include "main.h"

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define DBG_LED_A DT_NODELABEL(dbg_led_a)
static const struct gpio_dt_spec led_a = GPIO_DT_SPEC_GET(DBG_LED_A, gpios);
#define DBG_LED_B DT_NODELABEL(dbg_led_b)
static const struct gpio_dt_spec led_b = GPIO_DT_SPEC_GET(DBG_LED_B, gpios);
#define DBG_LED_C DT_NODELABEL(dbg_led_c)
static const struct gpio_dt_spec led_c = GPIO_DT_SPEC_GET(DBG_LED_C, gpios);

#define DBG_GPIO DT_NODELABEL(dbg_gpio)
static const struct gpio_dt_spec dbg_gpio = GPIO_DT_SPEC_GET(DBG_GPIO, gpios);

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
	bool ledOkay = true;
	bool mcp9808Okay = true;
	bool dbgGpioOkay = true;
	uint32_t counter = 0UL;

	if (mcp9808 == NULL) {
		LOG_ERR("MCP9808 not found");
		mcp9808Okay = false;
	}
	if (!device_is_ready(mcp9808)) {
		LOG_ERR("MCP9808 not ready");
		mcp9808Okay = false;
	}

	if (!device_is_ready(norflash)) {
		LOG_ERR("FLASH device not ready");
		return 0;
	}

	if (gpio_is_ready_dt(&led_a)) {
		ret = gpio_pin_configure_dt(&led_a, GPIO_OUTPUT_INACTIVE);
		if (ret < 0) {
			LOG_ERR("Could not configure DBG LED A");
			ledOkay = false;
		}
	}

	if (gpio_is_ready_dt(&led_b)) {
		ret = gpio_pin_configure_dt(&led_b, GPIO_OUTPUT_INACTIVE);
		if (ret < 0) {
			LOG_ERR("Could not configure DBG LED B");
			ledOkay = false;
		}
	}

	if (gpio_is_ready_dt(&led_c)) {
		ret = gpio_pin_configure_dt(&led_c, GPIO_OUTPUT_INACTIVE);
		if (ret < 0) {
			LOG_ERR("Could not configure DBG LED C");
			ledOkay = false;
		}
	}

	if (gpio_is_ready_dt(&dbg_gpio)) {
		ret = gpio_pin_configure_dt(&dbg_gpio, GPIO_OUTPUT_INACTIVE);
		if (ret < 0) {
			LOG_ERR("Could not configure DBG GPIO");
			dbgGpioOkay = false;
		}
	}

#ifdef SELF_TEST
	single_sector_test(norflash);
#endif

	while (1) {
		for (int i = 0; i < 3; i++) {
			if (!ledOkay) {
				continue;
			}
			if (i == 0) {
				(void)gpio_pin_toggle_dt(&led_a);
			} else if (i == 1) {
				(void)gpio_pin_toggle_dt(&led_b);
			} else if (i == 2) {
				(void)gpio_pin_toggle_dt(&led_c);
			}
			k_sleep(K_MSEC(100));
		}

		if (mcp9808Okay) {
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

		if (dbgGpioOkay) {
			(void)gpio_pin_toggle_dt(&dbg_gpio);
		}

		LOG_INF("Counter: %" PRIu32, counter++);
	}
}
