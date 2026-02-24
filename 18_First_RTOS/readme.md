# #18 First RTOS

A simple **FreeRTOS dual-task synchronization example** for ESP32 using the Arduino framework.  
This project demonstrates safe access to a shared resource (Serial output) using a **mutex**.

## Overview

The system creates two FreeRTOS tasks running with the same priority. Both tasks attempt to print data to the Serial Monitor.
Since Serial is a shared resource, a **mutex** is used to prevent race conditions and ensure that only one task prints at a time.

## Key Features

- Two concurrent FreeRTOS tasks
- Mutex-based resource protection
- Same-priority round-robin scheduling
- Demonstration of polling vs blocking behavior
- Runs on ESP32 (Arduino + ESP-IDF FreeRTOS)

## How It Works

1. The ESP32 initializes the Serial interface.
2. A mutex is created to guard access to Serial.
3. Two tasks are started with the same priority.
4. Each task:
   - Attempts to take the mutex
   - Prints a sequence of characters
   - Releases the mutex

When the mutex is used correctly, the output remains ordered and consistent.
