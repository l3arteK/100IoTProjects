# ESP32 Edge AI Climate Classifier

Simple environmental condition classifier using **ESP32**, **Edge Impulse**, and a **DHT11** sensor.
The sensor reads temperature and humidity, which are passed to a TinyML model that classifies the current environmental condition into one of 5 categories.

## Features

- Real-time temperature and humidity reading via DHT11
- On-device ML inference using Edge Impulse EON Compiler
- 5-class environmental classification
- Serial output with per-class confidence scores
- Runs entirely on ESP32 — no cloud required

## How It Works

Every 3 seconds the system:

1. Reads `temperature` and `humidity` from the DHT11 sensor
2. Packs the two values into an input buffer
3. Runs the Edge Impulse classifier
4. Prints all class scores and highlights the top result

Example Serial output:

```
Temp: 24.5 C, Hum: 61.0 %
  class_a: 0.0312
  class_b: 0.8741
  class_c: 0.0421
  class_d: 0.0312
  class_e: 0.0214
>> Result: class_b (87.41%)
```

## Model Details

| Parameter          | Value                                |
| ------------------ | ------------------------------------ |
| Input axes         | temperature + humidity               |
| Input frame size   | 2                                    |
| Output classes     | 5                                    |
| Inferencing engine | EON Compiler (TFLite quantized INT8) |
| Project            | l3artek-project-1                    |

## Circuit Image

![Circuit image](circuit_images/circuit_image.png)

## Hardware Requirements

Required components:

- ESP32 DevKit
- DHT11 temperature & humidity sensor

## Pin Configuration

| Component  | ESP32 Pin |
| ---------- | --------- |
| DHT11 DATA | GPIO 4    |

## Software Requirements

### PlatformIO dependencies

```ini
lib_deps =
    adafruit/Adafruit Unified Sensor@^1.1.15
    adafruit/DHT sensor library@^1.4.6
```

### Edge Impulse SDK

The following folders must be present in `lib/`:

```
lib/
├── edge-impulse-sdk/
├── model-parameters/
└── tflite-model/
```

Export them from the Edge Impulse Studio under **Deployment → C++ Library**.

### platformio.ini

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
lib_archive = no
lib_ldf_mode = deep+

build_flags =
    -std=gnu++14
    -I lib/
    -I lib/edge-impulse-sdk
    -I lib/edge-impulse-sdk/classifier
    -I lib/edge-impulse-sdk/porting
    -I lib/model-parameters
    -I lib/tflite-model
    -D EI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=0
    -D EI_CLASSIFIER_TFLITE_ENABLE_ESP_DSP=0
    -D EON_EDGES_PORT_ARDUINO=1

build_src_filter =
    +<../src/>
    +<../lib/edge-impulse-sdk/porting/espressif/>
    +<../lib/tflite-model/>
    +<../lib/edge-impulse-sdk/tensorflow/lite/micro/>
    +<../lib/edge-impulse-sdk/tensorflow/lite/micro/kernels/>
    +<../lib/edge-impulse-sdk/tensorflow/lite/kernels/>
    +<../lib/edge-impulse-sdk/tensorflow/lite/c/>
    +<../lib/edge-impulse-sdk/tensorflow/lite/core/api/>

lib_deps =
    adafruit/Adafruit Unified Sensor@^1.1.15
    adafruit/DHT sensor library@^1.4.6
```

## Project Structure

```
project/
├── src/
│   └── main.cpp
├── lib/
│   ├── edge-impulse-sdk/
│   ├── model-parameters/
│   └── tflite-model/
├── include/
└── platformio.ini
```

## Task Flow

```
loop() every 3s
│
├── Read DHT11 → temperature, humidity
├── Validate readings (NaN check)
├── Pack into float[2] input buffer
├── signal_from_buffer()
├── run_classifier()
├── Print all 5 class scores
└── Print top result
```
