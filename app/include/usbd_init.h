#pragma once

#include <zephyr/usb/usbd.h>

struct usbd_context *usbd_init_device(usbd_msg_cb_t msg_cb);
