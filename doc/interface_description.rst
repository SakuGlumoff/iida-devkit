Interface description
#####################

Command task
============

The command task informs other tasks of user button events using the ``extern struct k_event user_button_event`` specified in the command task header.

The event bits (``enum user_button_event_bits``) are defined as follows:
  - ``USER_BUTTON_SHORT_PRESS``: A short press of the user button (0x1).
  - ``USER_BUTTON_DOUBLE_PRESS``: A double press of the user button (0x2).
  - ``USER_BUTTON_LONG_PRESS``: A long press of the user button (0x3).

The command task informs other tasks of some command inputs from the USB CDC-ACM pseudo-shell using the ``extern struct k_event command_event`` specified in the command task header.

The event bits (``enum command_event_bits``) are defined as follows:
  - ``COMMAND_RESET``: Request to reset the system (0x1).
  - ``COMMAND_GET_SENSOR``: Request to get the current sensor values (0x2).
  - ``COMMAND_GET_SENSOR_ID``: Request to get the current value of a specific sensor (0x3).
  - ``COMMAND_GET_BATTERY``: Request to get the current battery status (0x4).
  - ``COMMAND_GET_IMEI``: Request to get the IMEI of the system (0x5).
  - ``COMMAND_GET_IP``: Request to get the current IP address of the modem (0x6).
  - ``COMMAND_GET_MODEM_INFO``: Request to get the modem information (0x7).

The other tasks will then respond to these events by performing the requested actions and sending the results back to the command task using the following functions specified in the command task header:
  - ``void command_task_sensor_update(sensor_id_t sensor_id, int32_t value)``: Updates the value of a specific sensor.
  - ``void command_task_battery_status_update(uint32_t voltage, uint32_t percentage)``: Updates the battery status with the current voltage and percentage.
  - ``void command_task_imei_update(uint8_t* imei, uint32_t imei_len)``: Updates the IMEI of the system.
  - ``void command_task_ip_update(uint8_t* ip, uint32_t ip_len)``: Updates the current IP address of the modem.
  - ``void command_task_modem_info_update(uint8_t rssi, modem_state_t state)``: Updates the modem information.

Sensor task
===========

TODO

Battery task
============

TODO

IO expander task
================

TODO

Network task
============

TODO
