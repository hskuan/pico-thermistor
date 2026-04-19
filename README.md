# picothermistor

Raspberry Pi Pico W firmware that reads an NTC100K thermistor via ADC, prints temperature over USB serial, and publishes readings to an MQTT broker every 5 seconds.

## Hardware

- Raspberry Pi Pico W
- NTC100K thermistor in a voltage divider with a 100 kΩ fixed resistor
  - Fixed resistor from 3.3 V to GPIO26 (physical pin 31)
  - Thermistor from GPIO26 to GND
- USB cable (powers the board and carries serial data)

## Dependencies

| Tool | Purpose |
|------|---------|
| [VS Code](https://code.visualstudio.com/) | Editor |
| [PlatformIO IDE extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) | Build system, uploading, serial monitor |
| Git | Required by PlatformIO to fetch the Arduino-Pico platform |
| Mosquitto (or any MQTT broker) | Receives temperature readings |

PlatformIO downloads all compiler toolchains, the Arduino-Pico framework, and the PubSubClient library automatically on first build.

> **Note:** The official PlatformIO `raspberrypi` platform does not support the Pico W's board ID (`rpipicow`). This project uses the [maxgerhardt/platform-raspberrypi](https://github.com/maxgerhardt/platform-raspberrypi) wrapper, which bundles the earlephilhower Arduino-Pico core and does support it.

## Setup

### 1. Install VS Code and PlatformIO

Install VS Code, then install the **PlatformIO IDE** extension from the Extensions panel (`Ctrl+Shift+X`).

### 2. Clone the repository

```bash
git clone https://github.com/hskuan/pico-thermistor.git
cd pico-thermistor
```

### 3. Create your config file

```bash
cp include/config.h.example include/config.h
```

Edit `include/config.h` and fill in your WiFi and MQTT credentials:

```cpp
#define WIFI_SSID      "your-ssid"
#define WIFI_PASSWORD  "your-password"

#define MQTT_BROKER    "192.168.1.x"
#define MQTT_PORT      1883
#define MQTT_TOPIC     "sensors/thermistor/temperature"
#define MQTT_CLIENT_ID "pico-thermistor"
#define MQTT_USER      "your-mqtt-username"
#define MQTT_PASSWORD  "your-mqtt-password"
```

`config.h` is gitignored and will never be committed.

### 4. MQTT broker setup (Mosquitto on Debian/Ubuntu)

If you don't have a user set up yet:

```bash
sudo mosquitto_passwd -c /etc/mosquitto/passwd <username>
```

Ensure `/etc/mosquitto/conf.d/auth.conf` contains:

```
allow_anonymous false
password_file /etc/mosquitto/passwd
```

Then restart the broker:

```bash
sudo systemctl restart mosquitto
```

### 5. Open in VS Code

```
File > Open Folder > select the pico-thermistor folder
```

PlatformIO will detect `platformio.ini` automatically.

### 6. Build (first build downloads the platform — takes a few minutes)

```bash
pio run
```

Or use the PlatformIO toolbar at the bottom of VS Code (checkmark icon).

### 7. Fix IntelliSense (Arduino.h not found)

After the first build completes, regenerate the IntelliSense index so `#include <Arduino.h>` resolves:

1. Open the Command Palette (`Ctrl+Shift+P`)
2. Run **PlatformIO: Rebuild IntelliSense Index**

### 8. Upload to the Pico W

Hold the **BOOTSEL** button on the Pico W while plugging in USB, then release it. The board mounts as a USB drive. Then run:

```bash
pio run --target upload
```

### 9. Monitor serial output

```bash
pio device monitor
```

Output (115200 baud):
```
Connecting to WiFi... connected
Connecting to MQTT connected
Temp: 22.46 C
Temp: 22.20 C
...
```

### 10. Subscribe to MQTT topic

On the broker or any machine on the network:

```bash
mosquitto_sub -h 192.168.1.x -u <username> -P <password> -t sensors/thermistor/temperature
```

## Project structure

```
src/main.cpp              — ADC read, Beta equation, WiFi/MQTT, serial output
include/config.h          — your credentials (gitignored)
include/config.h.example  — template to copy from
platformio.ini            — board, platform, framework, library config
```

## Thermistor math

Temperature is calculated from the ADC reading using the Beta equation:

```
1/T = 1/T₀ + (1/B) × ln(R_ntc / R₀)
```

| Constant | Value | Description |
|----------|-------|-------------|
| `T₀` | 298.15 K | Nominal temperature (25 °C) |
| `R₀` | 100 000 Ω | Nominal thermistor resistance at T₀ |
| `B` | 3950 K | Beta coefficient (verify against your datasheet) |
| `R_fixed` | 100 000 Ω | Fixed resistor in the voltage divider |
