#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/usb/usbd.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(app, CONFIG_APP_LOG_LEVEL);

/* By default, do not register the USB DFU class DFU mode instance. */
static const char *const _blocklist[] = {
	"dfu_dfu",
	NULL,
};

// Use STMicroelectronics vendor ID
#define USBD_VID          (0x0483)
// Use a random product ID
#define USBD_PID          (0x0001)
#define USBD_MANUFACTURER "SakuGlumoff"
#define USBD_PRODUCT      "Iida devkit"
#define USBD_MAX_POWER    (500) /* mA */

/*
 * Instantiate a context named usbd_ctx using the default USB device
 * controller, the Zephyr project vendor ID, and the sample product ID.
 * Zephyr project vendor ID must not be used outside of Zephyr samples.
 */
USBD_DEVICE_DEFINE(usbd_ctx, DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)), USBD_VID, USBD_PID);

USBD_DESC_LANG_DEFINE(lang_desc);
USBD_DESC_MANUFACTURER_DEFINE(mfr_desc, USBD_MANUFACTURER);
USBD_DESC_PRODUCT_DEFINE(product_desc, USBD_PRODUCT);
USBD_DESC_CONFIG_DEFINE(fs_cfg_desc, "FS Configuration");
USBD_DESC_CONFIG_DEFINE(hs_cfg_desc, "HS Configuration");

static const uint8_t attributes = USB_SCD_SELF_POWERED;

USBD_CONFIGURATION_DEFINE(fs_cfg, attributes, USBD_MAX_POWER / 2 /* 2 mA units */, &fs_cfg_desc);
USBD_CONFIGURATION_DEFINE(hs_cfg, attributes, USBD_MAX_POWER / 2 /* 2 mA units */, &hs_cfg_desc);

static void _fix_code_triple(struct usbd_context *ctx, const enum usbd_speed speed)
{
	/* Always use class code information from Interface Descriptors */
	if (IS_ENABLED(CONFIG_USBD_CDC_ACM_CLASS) || IS_ENABLED(CONFIG_USBD_CDC_ECM_CLASS) ||
	    IS_ENABLED(CONFIG_USBD_CDC_NCM_CLASS) || IS_ENABLED(CONFIG_USBD_MIDI2_CLASS) ||
	    IS_ENABLED(CONFIG_USBD_AUDIO2_CLASS) || IS_ENABLED(CONFIG_USBD_VIDEO_CLASS)) {
		/*
		 * Class with multiple interfaces have an Interface
		 * Association Descriptor available, use an appropriate triple
		 * to indicate it.
		 */
		usbd_device_set_code_triple(ctx, speed, USB_BCC_MISCELLANEOUS, 0x02, 0x01);
	} else {
		usbd_device_set_code_triple(ctx, speed, 0, 0, 0);
	}
}

static struct usbd_context *usbd_setup_device(usbd_msg_cb_t cb)
{
	int err;

	err = usbd_add_descriptor(&usbd_ctx, &lang_desc);
	if (err) {
		LOG_ERR("Failed to initialize language descriptor (%d)", err);
		return NULL;
	}

	err = usbd_add_descriptor(&usbd_ctx, &mfr_desc);
	if (err) {
		LOG_ERR("Failed to initialize manufacturer descriptor (%d)", err);
		return NULL;
	}

	err = usbd_add_descriptor(&usbd_ctx, &product_desc);
	if (err) {
		LOG_ERR("Failed to initialize product descriptor (%d)", err);
		return NULL;
	}

	if (USBD_SUPPORTS_HIGH_SPEED && usbd_caps_speed(&usbd_ctx) == USBD_SPEED_HS) {
		err = usbd_add_configuration(&usbd_ctx, USBD_SPEED_HS, &hs_cfg);
		if (err) {
			LOG_ERR("Failed to add High-Speed configuration");
			return NULL;
		}

		err = usbd_register_all_classes(&usbd_ctx, USBD_SPEED_HS, 1, _blocklist);
		if (err) {
			LOG_ERR("Failed to add register classes");
			return NULL;
		}

		_fix_code_triple(&usbd_ctx, USBD_SPEED_HS);
	}

	err = usbd_add_configuration(&usbd_ctx, USBD_SPEED_FS, &fs_cfg);
	if (err) {
		LOG_ERR("Failed to add Full-Speed configuration");
		return NULL;
	}

	err = usbd_register_all_classes(&usbd_ctx, USBD_SPEED_FS, 1, _blocklist);
	if (err) {
		LOG_ERR("Failed to add register classes");
		return NULL;
	}

	_fix_code_triple(&usbd_ctx, USBD_SPEED_FS);
	usbd_self_powered(&usbd_ctx, attributes & USB_SCD_SELF_POWERED);

	if (cb != NULL) {
		err = usbd_msg_register_cb(&usbd_ctx, cb);
		if (err) {
			LOG_ERR("Failed to register message callback");
			return NULL;
		}
	}

	return &usbd_ctx;
}

struct usbd_context *usbd_init_device(usbd_msg_cb_t cb)
{
	int err;

	if (usbd_setup_device(cb) == NULL) {
		return NULL;
	}

	err = usbd_init(&usbd_ctx);
	if (err) {
		LOG_ERR("Failed to initialize device support");
		return NULL;
	}

	return &usbd_ctx;
}
