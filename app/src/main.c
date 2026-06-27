#include <zephyr/kernel.h>
#include <zephyr/app_version.h>
#include <zephyr/version.h>
#include <zephyr/autoconf.h>

#include "main.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

int main(void)
{
	LOG_INF("Iida devkit application: %s %s\n", APP_VERSION_STRING,
		STRINGIFY(APP_BUILD_VERSION));
	LOG_INF("Zephyr: %s\n", STRINGIFY(BUILD_VERSION));
	LOG_INF("Board: %s\n", CONFIG_BOARD_TARGET);
	LOG_INF("Built on %s @ %s\n", __DATE__, __TIME__);
}
