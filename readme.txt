000 - LCD Library

This file contains a custom library for interfacing with a 16x2 LCD module. It provides:

Functions to initialize the LCD.

Utilities to print text and control the display.

Cursor manipulation and display settings.

This library is used in other projects in the repository.


001 - LED Blinking with Switch Control

This project demonstrates:

Configuring GPIO pins for input and output.

Controlling RGB LEDs based on switch inputs.

Adjusting LED blinking speed using push buttons.


002 - Digital Clock

A digital clock implementation that:

Uses timer interrupts to update the time.

Displays hours, minutes, and seconds on a 16x2 LCD.

Demonstrates effective use of the LCD library from LiquidCrystal1602.c


003 - SharpDevolp

A desktop application developed to interact with the microcontroller via a serial port. It includes:

A user-friendly interface for sending and receiving data.

Serial communication functionalities for real-time control and monitoring.


004 - UART and RGB LED Control

This project combines serial communication and LED control:

Receives commands via UART to control RGB LEDs.

Demonstrates parsing and processing serial input.

Includes a digital clock feature synchronized through received data.


005 - ADC and Temperature Monitoring

This project integrates ADC and UART features:

Reads temperature using the internal temperature sensor.

Displays the temperature and time on an LCD.

Sends and receives data via UART.

Uses RGB LEDs for status indication.