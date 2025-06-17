Interface description
#####################

This document describes the interface between the various tasks in the system.

The tasks are assigned different priority levels being one of [ `HIGH`, `MEDIUM`, `LOW` ].

Command task
============

The priority of the command task is `HIGH`.

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

The priority of the sensor task is `LOW`.

The sensor task informs other tasks of sensor events using the ``extern struct k_event sensor_event`` specified in the sensor task header.

The event bits (``enum sensor_event_bits``) are defined as follows:
  - ``UPDATE_TEMP_SENSOR``: The temperature sensor has been updated (0x1).
  - ``UPDATE_HUMIDITY_SENSOR``: The humidity sensor has been updated (0x2).
  - ``ALERT_TEMP_SENSOR``: The temperature sensor has detected an alert condition (0x3).
  - ``ALERT_HUMIDITY_SENSOR``: The humidity sensor has detected an alert condition (0x4).

The other tasks may then read the sensor values using the following functions specified in the sensor task header:
  - ``float sensor_task_get_temperature(void)``: Returns the current temperature value.
  - ``float sensor_task_get_humidity(void)``: Returns the current humidity value.

The sensor task handles the configuration and control of the temperature and humidity sensors.

Battery task
============

The priority of the battery task is `LOW`.

The battery task informs other tasks of battery events using the ``extern struct k_event battery_event`` specified in the battery task header.

The event bits (``enum battery_event_bits``) are defined as follows:
  - ``UPDATE_BATTERY_STATUS``: The battery status has been updated (0x1).
  - ``ALERT_BATTERY_LOW``: The battery is low (0x2).

The other tasks may then read the battery status using the following functions specified in the battery task header:
  - ``uint32_t battery_task_get_voltage(void)``: Returns the current battery voltage.
  - ``uint32_t battery_task_get_percentage(void)``: Returns the current battery percentage.

The battery task handles the configuration and control of the battery monitor.

IO expander task
================

The priority of the IO expander task is `LOW`.

The IO expander task informs other tasks of IO events using the ``extern struct k_event io_event`` specified in the IO expander task header.

The event bits (``enum io_event_bits``) are defined as follows:
  - ``TEMP_SENSOR_ALERT``: The temperature sensor has detected an alert condition (0x1).
  - ``BATTERY_LOW_ALERT``: The battery monitor has detected a low battery condition (0x2).

The other tasks may use the following functions to control the IO expander:
  - ``void io_expander_task_set_led(enum indicator_led led, bool state)``: Sets the state of a specific LED.
  - ``void io_expander_task_modem_reset(bool state, uint32_t time)``: Resets the modem.
  - ``void io_expander_task_modem_power_enable(bool state)``: Enables the modem on or off.
  - ``void io_expander_task_modem_power_ctrl(bool state)``: Controls the modem power pin.

The IO expander task handles control of the IO expander and also clearing the interrupts from the IO expander.

Network task
============

The priority of the network task is `MEDIUM`.

The other tasks may query modem information using the following functions specified in the network task header:
  - ``modem_state_t network_task_get_modem_state(void)``: Returns the current state of the modem.
  - ``uint8_t network_task_get_modem_rssi(void)``: Returns the current RSSI value of the modem.
  - ``uint8_t* network_task_get_modem_imei(void)``: Returns the current IMEI of the modem.
  - ``uint8_t* network_task_get_modem_ip(void)``: Returns the current IP address of the modem.

The network task monitors for sensor and battery events and sends updates about them to the backend server.
