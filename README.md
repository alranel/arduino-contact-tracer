# arduino-contact-tracer

This is an implementation for Arduino of the contact tracing spec defined by [Apple](https://www.apple.com/covid19/contacttracing/) and [Google](https://blog.google/inside-google/company-announcements/apple-and-google-partner-covid-19-contact-tracing-technology).

While the contact tracing technology is focused on smartphones, it may make sense to create additional tracing devices to be placed for example in stores or workplaces where one cannot assume that workers carry their phones in their pocket or keep the tracing feature enabled, or where multiple location granularity is useful.

Note that according to individual governments' policies, a smartphone app might still be necessary to publish or download daily keys corresponding to people who get diagnosed positively.

## Hardware

This (untested) code runs on the [Arduino Nano 33 BLE](https://store.arduino.cc/usa/nano-33-ble) which is a good candidate for this task because of:

* small form factor
* low price
* support of BLE 5.1
* large flash memory (1MB) allowing about 50k+ entries
* is based on Mbed OS which provides the needed crypto functions

No more hardware is required, unless one wants to perform logging to a SD card instead of the flash memory.

## Status

As of now, this repo contains the implementation of the cryptographic functions (generation of the Tracing Key, the Daily Tracing Key and the Rolling Proximity Identifier).

Examples of **advertising services** or **scanning services** showing how to use the [ArduinoBLE library](https://www.arduino.cc/en/Reference/ArduinoBLE) are still missing (contributions welcome!).

The spec requires **access to the current Unix epoch time**. Since the Arduino Nano 33 BLE does not support Wi-fi, it is hard-coded here. We could implement the Current Time Service described by the Bluetooth spec and pair it with a smartphone, or we might want to consider porting the code to an [Arduino Nano 33 IoT](https://store.arduino.cc/usa/nano-33-iot) which has Wi-fi support.
