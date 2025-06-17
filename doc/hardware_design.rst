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

Memory
----------

The memory map of the STM32L552 microcontroller is organized into several regions, each serving a specific purpose. The memory layout is as follows:

.. graphviz::

   digraph memory_map {
     rankdir=TB;
     node [shape=record, width=2.5, height=0.6, fontsize=12];
     nodesep=0;
     ranksep=0;

     ROM   [label="{ ROM | 0x8000'0000 - 0x8000'FFFF }", style=filled, fillcolor="#e6f2ff"];
     USER  [label="{ USER | 0x8001'0000 - 0x8001'FFFF }", style=filled, fillcolor="#fff2cc"];
	 RTT   [label="{ RTT | 0x2000'0000 - 0x2000'0FFF }", style=filled, fillcolor="#ffe6e6"];
     RAM   [label="{ RAM | 0x2000'1000 - 0x2000'FFFF }", style=filled, fillcolor="#e6ffe6"];

     RAM -> RTT -> USER -> ROM [style=invis];
   }

The program code is stored in the ROM region, while user-specific data is stored in the USER region.

The RTT (Real-Time Trace) block is used for debugging and tracing, and it occupies a small portion of the RAM region. The RTT block is configured to reside at the start of the RAM region, which is the default location for RTT in STM32 microcontrollers.

The main RAM region is used for runtime data storage.

Debugging
---------

Debugging is facilitated through the SWD (Serial Wire Debug) interface, which allows for programming and debugging of the microcontroller. The SWD interface is accessible via the Tag-Connect 2050 header (J502) on the board.

An RTT (Real-Time Trace) block is available for advanced debugging and tracing capabilities. This interface allows for real-time monitoring of the system's operation using Segger's JLink, which is useful for performance analysis and debugging complex issues.

Reset
-----

When the reset functionality is triggered, either by the user pressing the reset button or by a using the debugger, the microcontroller resets and initializes all peripherals.

If the reset is triggered by the user inputting a command via the USB interface, the modem is also reset.

First the microcontroller boots up and initializes itself, setting up the clock and memory systems. It then initializes the peripherals, including the modem and sensors.

Clocks
------

The device uses multiple clock sources to ensure accurate timing and operation of the microcontroller and peripherals.

The main clock source is the external crystal oscillator (HSE), which provides a stable frequency for the system clock.

The microcontroller also has an internal high-speed oscillator (HSI) that can be used as a backup clock source. The system clock is configured to run at 100 MHz, derived from the HSE.

To support RTC (Real-Time Clock) functionality, the device uses a low-speed external crystal oscillator (LSE) for accurate timekeeping. The LSE is a 32.768 kHz crystal.

Hardware identification
-----------------------

To differentiate between different hardware revisions, the device uses a hardware ID made up of pull-up/pull-down resistors on specific GPIO pins. The hardware ID is read during the boot process and can be used to determine the hardware revision.

User button
-----------

The user button is connected to PA9 through a solder jumper. The user can use the button according to their needs, such as triggering a reset or initiating a specific action in the firmware.

If the user button is not needed, the solder jumper can be unsoldered to free up the GPIO pin for other purposes.

Indicator LEDs
--------------

To indicate the status of the device, there are three indicator LEDs connected to the IO expander. The usage of the LEDs is up to the user.

The LEDs' cathodes are connected to the IO expander's ports 0, 1, and 2, while the anodes are connected to the 3.3V power rail. The IO expander controls the LEDs by pulling the respective port low to turn on the LED.

Sensors
=======

Ambient temperature sensor
--------------------------

The device includes an ambient temperature sensor, which is a digital temperature sensor connected via I2C. The sensor provides accurate temperature readings and can be used for environmental monitoring.

The used sensor is the MCP9808, which is a high-accuracy digital temperature sensor with a range of -40°C to +125°C and an accuracy of ±0.25°C at room temperature.

The I2C address of the MCP9808 is 0x18, and it can be configured to operate in different modes, such as continuous conversion mode or one-shot mode.

Humidity sensor
---------------

The device includes a humidity sensor, which is also connected via I2C. The sensor provides accurate humidity readings and can be used for environmental monitoring.

The used sensor is the SHT40, which is a high-accuracy digital humidity and temperature sensor with a range of 0% to 100% relative humidity and an accuracy of ±1.5% RH.

The I2C address of the SHT40 is 0x44.

Battery monitoring
------------------

The device includes a battery monitoring system that measures the battery voltage and current. This information is used to manage power consumption and ensure safe operation of the device. For example, if the battery voltage drops below a certain threshold, the device can enter a low-power mode or shut down to prevent damage to the battery.

The used battery monitoring IC is the MAX17048, which is a battery fuel gauge IC that provides accurate battery voltage and current measurements. It communicates with the microcontroller via I2C.

The I2C address of the MAX17048 is 0x6C.

Modem
=====

Modem power
-----------

The power to the modem is controlled by port 5 of the IO expander, which is connected to a low-side NMOS, which controls the high-side PMOS. The modem is powered on when the IO expander port 5 is set to high, and it is powered off when the port is set to low.

By default, the modem is not powered on, and it is only powered on when the user sends a command via the USB interface or when the device is reset.

Modem control
-------------

The modem is controlled via the port 3 (modem power on control) and port 4 (modem reset control) of the IO expander. The modem power on control port is used to turn the modem on and off, while the modem reset control port is used to reset the modem.

As the modem's control signals work with 1.8V logic levels, the IO expander controls the gate pins of NMOS transistors, which in turn control the modem's power and reset signals.

For communicating with the modem, the microcontroller is connected to the modem via the logic-level shifter, which is configured to operate in 1.8V mode. The UART interface is configured to 115200 baud, 8 data bits, no parity, and 1 stop bit (115200 8N1).
The microcontroller's modem UART interface TX pin is PB6 and the RX pin is PB7.

SIM card
--------

To allow the modem to connect to a cellular network, the device includes a SIM card slot. The SIM card is used to authenticate the device on the network and provide access to cellular services.

The SIM card slot is a standard nano SIM card slot, which is compatible with most modern SIM cards.

The SIM card is not included with the device, and the user must provide their own SIM card to use the cellular functionality.

The modem supports hot-swapping of the SIM card, allowing the user to change the SIM card without powering off the device. The modem will automatically detect the new SIM card and reinitialize itself.

This is accomplished by having the SIM card slot's normally closed (to ground) detect pin pulled up to 1.8V via a pull-up resistor. When the SIM card is inserted, the detect pin is opened causing the line to go high, indicating to the modem that a SIM card is present. When the SIM card is removed, the detect pin is pulled low again, indicating to the modem that no SIM card is present.

Antenna
-------

See :doc:`antenna_design`
