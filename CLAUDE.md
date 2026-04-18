# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

PlatformIO project for a Raspberry Pi Pico W that reads temperature from an NTC100K thermistor and reports values over USB serial.

## Build & Flash Commands

```bash
# Build the project
pio run

# Upload to connected Pico W (hold BOOTSEL button or use picotool)
pio run --target upload

# Monitor serial output (default 115200 baud)
pio device monitor

# Build + upload + monitor in one step
pio run --target upload && pio device monitor

# Clean build artifacts
pio run --target clean
```

## Project Structure

- `platformio.ini` — board config (`rpipicow`), framework (`arduino`), serial monitor speed
- `src/main.cpp` — entry point; setup/loop with ADC read and serial output
- `lib/` — any local libraries (e.g. thermistor math helpers)
- `include/` — shared headers

## Hardware

- **Board:** Raspberry Pi Pico W (`rpipicow`)
- **Framework:** Arduino (via PlatformIO)
- **Thermistor:** NTC100K in a voltage divider; ADC pin reads the midpoint
- **Output:** USB serial at 115200 baud

## Thermistor Math

NTC resistance is derived from the ADC reading via a voltage divider (typically 100kΩ fixed resistor + NTC to 3.3 V and GND). Temperature is calculated using the Steinhart-Hart equation or the simplified Beta equation:

```
1/T = 1/T0 + (1/B) * ln(R/R0)
```

- `T0` = 298.15 K (25 °C nominal)
- `R0` = 100,000 Ω
- `B`  = ~3950 K (verify against your specific thermistor datasheet)

## platformio.ini Baseline

```ini
[env:rpipicow]
platform  = raspberrypi
board     = rpipicow
framework = arduino
monitor_speed = 115200
```
