# picothermistor

Raspberry Pi Pico W firmware that reads an NTC100K thermistor via ADC and prints temperature over USB serial at 1 Hz.

## Hardware

- Raspberry Pi Pico W
- NTC100K thermistor in a voltage divider with a 100 kΩ fixed resistor
  - Fixed resistor from 3.3 V to the midpoint (ADC pin 26)
  - Thermistor from the midpoint to GND
- USB cable (powers the board and carries serial data)

## Dependencies

| Tool | Purpose |
|------|---------|
| [VS Code](https://code.visualstudio.com/) | Editor |
| [PlatformIO IDE extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) | Build system, uploading, serial monitor |
| Git | Required by PlatformIO to fetch the Arduino-Pico platform |

PlatformIO downloads all compiler toolchains and the Arduino-Pico framework automatically on first build — nothing else to install manually.

> **Note:** The official PlatformIO `raspberrypi` platform does not support the Pico W's board ID (`rpipicow`). This project uses the [maxgerhardt/platform-raspberrypi](https://github.com/maxgerhardt/platform-raspberrypi) wrapper, which bundles the earlephilhower Arduino-Pico core and does support it.

## Setup

### 1. Install VS Code and PlatformIO

Install VS Code, then install the **PlatformIO IDE** extension from the Extensions panel (`Ctrl+Shift+X`).

### 2. Clone the repository

```bash
git clone <repo-url>
cd picothermistor
```

### 3. Open in VS Code

```
File > Open Folder > select the picothermistor folder
```

PlatformIO will detect `platformio.ini` automatically.

### 4. Build (first build downloads the platform — takes a few minutes)

```bash
pio run
```

Or use the PlatformIO toolbar at the bottom of VS Code (checkmark icon).

### 5. Fix IntelliSense (Arduino.h not found)

After the first build completes, regenerate the IntelliSense index so `#include <Arduino.h>` resolves:

1. Open the Command Palette (`Ctrl+Shift+P`)
2. Run **PlatformIO: Rebuild IntelliSense Index**

### 6. Upload to the Pico W

Hold the **BOOTSEL** button on the Pico W while plugging in USB, then release it. The board mounts as a USB drive. Then run:

```bash
pio run --target upload
```

### 7. Monitor serial output

```bash
pio device monitor
```

Output (115200 baud):
```
Temp: 23.45 C
Temp: 23.47 C
...
```

## Project structure

```
src/main.cpp        — ADC read, Beta equation, serial output
platformio.ini      — board, platform, framework config
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
