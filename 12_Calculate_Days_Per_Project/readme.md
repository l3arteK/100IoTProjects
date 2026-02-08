# #12 Calculate Days Per Project

This project is a personal productivity tool designed for the **"100 Projects in One Year"** challenge. It helps track progress by calculating the remaining time available for each upcoming project based on the current date and the number of completed tasks.

## Features
* **Real-time Clock:** Fetches the current date from the internet using **NTP** (Network Time Protocol) via WiFi.
* **Dynamic Calculation:** Automatically computes the average time remaining per project until the final deadline.
* **Interactive UI:** A two-stage interface on an OLED display managed by physical buttons.
* **State Management:** Seamlessly switch between inputting the count of finished projects and viewing the result.

## How it Works
1. **Setup:** Upon booting, the ESP32 connects to WiFi and synchronizes its internal clock with a global NTP server.
2. **Input Mode (Mode 0):** The user uses the `PLUS` and `MINUS` buttons to set the number of projects already completed.
3. **Calculation:** When the `OK` button is pressed, the device calculates:
   $$Result = \frac{DeadlineSeconds - CurrentSeconds}{100 - ProjectsDone}$$
4. **Result Mode (Mode 1):** The OLED screen displays the exact number of days (with two decimal precision) allowed for each remaining project to meet the end-of-year goal.

## Circuit image
![Circuit image](circuit_images/circuit_image.png)


## Hardware Requirements
* **ESP32** DevKit V1
* **SSD1306** OLED Display (128x64) via $I^2C$ (SDA: GPIO 21, SCL: GPIO 22)
* **3x Tactile Push Buttons** (Plus, Minus, OK)

## Configuration
The project is currently set with the following parameters:
* **Timezone:** GMT+1 (Poland) with Daylight Saving Time support.
* **Target Deadline:** December 31, 2026.
* **Goal:** 100 projects total.
