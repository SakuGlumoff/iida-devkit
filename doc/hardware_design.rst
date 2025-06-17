Hardware design
###############

Power
=====

The device is powered either by a USB connection or by a battery. The power management system is designed to handle both sources efficiently and allow hot-swapping.

The battery is a rechargeable lithium-ion battery, which is charged via the USB connection when available. Due to the 450 mA charging current, the battery should be equal to or larger than 500 mAh capacity to ensure safe charging and operation.

Power sequence
--------------

.. wavedrom::

	{ "signal" :
		[
			{ "name": "VBUS",        "wave": "lh...." },
			{ "name": "BATT",        "wave": "l....." },
			{ "name": "V_SYS",       "wave": "l.h..." },
			{ "name": "V_3V7",       "wave": "l..h.." },
			{ "name": "V_3V3",       "wave": "l...h." },
			{ "name": "V_3V3_MODEM", "wave": "l....h" },
		],
		"head": {
			"text":
			[
				"tspan",
				[ "tspan", "USB plugged in" ],
			]
		},
		"config": { "hscale": 2 },
	}

.. wavedrom::

	{ "signal" :
		[
			{ "name": "VBUS",        "wave": "l....." },
			{ "name": "BATT",        "wave": "lh...." },
			{ "name": "V_SYS",       "wave": "l.h..." },
			{ "name": "V_3V7",       "wave": "l..h.." },
			{ "name": "V_3V3",       "wave": "l...h." },
			{ "name": "V_3V3_MODEM", "wave": "l....h" },
		],
		"head": {
			"text":
			[
				"tspan",
				[ "tspan", "Battery plugged in" ],
			]
		},
		"config": { "hscale": 2 },
	}

Inputs
======

The device has several inputs for user interaction and hardware control.

To reset the device, the user presses the reset button. This will reset the microcontroller and all peripherals, returning the device to its initial state.

To read system debug logs, the device has a non-populated UART header. The header pins are compatible with the USB-to-UART cables from FTDI. The UART interface is configured to 115200 baud, 8 data bits, no parity, and 1 stop bit (115200 8N1).

To command the device, the user can use the USB interface. The device will enumerate as a virtual serial port (CDC) and can be accessed using standard serial communication software.

Some timing related functionality is facilitated by a debug GPIO pin on a header. This pin can be used to trigger events or measure timing in the system.

To incorporate more peripherals (such as sensors), the device has a non-populated I2C header. This header allows the user to connect additional I2C devices to the system.

The device has an SMA connector for an external antenna. This allows the user to connect an antenna for better signal reception. The antenna is not included with the device. This allows the user to have a better tuned antenna to fit their LTE CAT-M1 band.

All unused GPIO pins are available on headers along the edges of the board.

Microcontroller
===============

The device is based on the STM32L552 microcontroller, which is a low-power ARM Cortex-M33 based MCU with a rich set of peripherals and features suitable for IoT applications.

The microcontroller is configured to run at a frequency of 100 MHz, with a system clock derived from the internal high-speed oscillator (HSI) and the external crystal oscillator (HSE) for precise timing.

The microcontroller acts as the central processing unit of the device, managing all operations, including communication with the modem, handling user inputs, and processing sensor data.

The microcontroller features:
  - 512 KB Flash memory
  - 256 KB SRAM

Memory map
----------

.. graphviz::

   digraph memory_map {
     rankdir=TB;
     node [shape=record, width=2.5, height=0.6, fontsize=12];
     nodesep=0;
     ranksep=0;

     ROM   [label="{ ROM | 0x8000'0000 - 0x8000'FFFF }", style=filled, fillcolor="#e6f2ff"];
     RAM   [label="{ RAM | 0x8001'0000 - 0x8001'FFFF }", style=filled, fillcolor="#e6ffe6"];
     USER  [label="{ USER | 0x8002'0000 - 0x8002'FFFF }", style=filled, fillcolor="#fff2cc"];

     USER -> RAM -> ROM [style=invis];
   }

Debugging
---------

TODO

Reset
-----

TODO

Clocks
------

TODO

Memory
------

TODO

Hardware identification
-----------------------

TODO

User button
-----------

TODO

Indicator LEDs
--------------

TODO

Sensors
=======

Ambient temperature sensor
--------------------------

TODO

Humidity sensor
---------------

TODO

Battery monitoring
------------------

TODO

Modem
=====

Modem power
-----------

TODO

Modem control
-------------

TODO

SIM card
--------

TODO

TODO: SIM card detection

Antenna
-------

See :doc:`antenna_design`
