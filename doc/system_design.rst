System design
#############

The system is divided into tasks that all have their own logical responsibility.

For example, the sensor task configures the sensors and handles their events, and the command task handles user requests from the USB interface.

Communication between the tasks is done via messages. For example, when the user requests a sensor value via the command interface,
the command task sends a message to request the sensor data and the sensor task responds by sending a reponse message with the sensor data.

Tasks
=====

Command task
------------

The command task handles all command inputs from the USB CDC-ACM pseudo-shell. It also handles user button presses.

Commands
++++++++

The command task supports the following commands:

- ``help``: Prints a list of available commands.
- ``version``: Prints the current firmware version.
- ``reset``: Resets the system.
- ``get_sensor``: Prints the current sensor values.
- ``get_sensor <sensor_id>``: Prints the current value of the specified sensor.
- ``get_battery``: Prints the current battery status.
- ``get_hwid``: Prints the hardware ID of the system.
- ``get_imei``: Prints the IMEI of the system.
- ``get_ip``: Gets the current IP address of the modem.
- ``get_modem_info``: Gets the modem information.
- ``get_logs``: Gets the logs from the system.
- ``clear_logs``: Clears the logs from the system.
- ``set_sim_pin <pin>``: Sets the SIM PIN for the modem.
- ``set_apn <apn>``: Sets the APN for the modem.
- ``set_password <password>``: Sets the password for the modem.
- ``ping <host>``: Pings the specified host.


User button
+++++++++++

The command task supports three different user button press types:

- A single press

  - Identified as a single falling edge of the user button signal in a two second timeframe.

- A double press

  - Identified as two falling edges of the user button signal within a two second timeframe.

- A long press

  - Identified as a single falling edge of the user button signal that lasts for at least two seconds.

Sensor task
-----------

The sensor task is responsible for controlling the ambient temperature and the humidity sensor.
It receives and handles sensor interrupt events from the IO expander task.
If the ambient temperature gets too high or too low, it sends a message about it to the command task.
If the humidity gets too high or too low, it also sends a message about it to the command task.

Battery task
------------

The battery task is responsible for monitoring the battery status and reacting to interrupt events from the battery monitor.
If the battery voltage drops below a certain threshold, it sends a message about it to the command task.

IO expander task
----------------

The IO expander task is responsible for controlling the IO expander.
Whenever the IO expander detects an interrupt event, it sends a message to the owner task (e.g., the sensor task or the battery task) to handle the event.
The IO expander task also handles and acts on request messages to control the modem.

Network task
------------

The network task is responsible for managing the modem, the network connection and handling network-related events.
