# DIY Advanced Oscilloscope on Arduino Uno

## Table of Contents
- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Usage](#usage)
  - [Serial Commands](#serial-commands)
  - [Waveform Display](#waveform-display)
  - [Measurements](#measurements)
- [Technical Details](#technical-details)
  - [ADC Configuration](#adc-configuration)
  - [Triggering System](#triggering-system)
  - [Sampling and Timebase](#sampling-and-timebase)
  - [Measurement Calculations](#measurement-calculations)
- [Limitations](#limitations)
- [Future Improvements](#future-improvements)


---

## Project Overview
This project implements an advanced oscilloscope using an Arduino Uno, displaying waveforms and measurements through the Serial Terminal. The oscilloscope captures analog signals, processes them, and provides real-time visualization using ASCII characters. It includes advanced features such as adjustable trigger levels, multiple trigger modes, and various signal measurements.

The project is designed for educational purposes, hobbyists, and basic signal analysis, with a focus on simplicity and accessibility.

---

## Features
- **Adjustable Trigger Level**: Set custom trigger thresholds (0-1023 ADC units).
- **Multiple Trigger Modes**:
  - Rising edge trigger
  - Falling edge trigger
  - Auto trigger (continuous display)
- **Voltage Scale Selection**: Adjustable voltage scale (1-10 V).
- **Timebase Adjustment**: Configurable sampling rate (10-1000 µs per sample).
- **Waveform Display**: ASCII-based waveform visualization (80 samples, 24 rows).
- **Measurements**:
  - Frequency (Hz)
  - Period (ms)
  - Amplitude (V, peak-to-peak)
  - RMS Voltage (V)
  - Duty Cycle (%)
- **Serial Command Interface**: Interactive control via Serial Terminal.
- **Display Freeze**: Toggle to freeze the waveform display for analysis.

---

## Hardware Requirements
- Arduino Uno (or compatible board)
- Analog signal source (e.g., function generator, sensor)
- USB cable for serial communication
- Optional: Voltage divider or protection circuit for signals > 5V

---

## Software Requirements
- Arduino IDE (version 1.8.x or later)
- Serial Terminal (built-in Arduino Serial Monitor or third-party tools like PuTTY)
- No additional libraries required

---

## Installation
1. **Clone or Download the Code**:
   - Download the `.ino` file or clone this repository to your local machine.
2. **Open in Arduino IDE**:
   - Open the `.ino` file in the Arduino IDE.
3. **Connect Arduino Uno**:
   - Connect your Arduino Uno to your computer via USB.
4. **Select Board and Port**:
   - In the Arduino IDE, select "Arduino Uno" as the board and the appropriate COM port.
5. **Upload the Code**:
   - Click "Upload" to compile and upload the code to the Arduino.
6. **Open Serial Monitor**:
   - Open the Serial Monitor (Tools > Serial Monitor) at 115200 baud rate to view the oscilloscope output.

---

## Usage

### Serial Commands
The oscilloscope supports interactive control via serial commands. Enter commands in the Serial Monitor and press Enter.

| Command                | Description                                      | Example                |
|------------------------|--------------------------------------------------|------------------------|
| `help`                 | Display available commands                       | `help`                 |
| `trigger <value>`      | Set trigger threshold (0-1023)                   | `trigger 512`          |
| `mode rising`          | Set rising edge trigger mode                     | `mode rising`          |
| `mode falling`         | Set falling edge trigger mode                    | `mode falling`         |
| `mode auto`            | Set auto trigger mode                            | `mode auto`            |
| `timebase <us>`        | Set timebase (10-1000 µs per sample)             | `timebase 100`         |
| `scale <volts>`        | Set voltage scale (1-10 V)                       | `scale 5.0`            |
| `freeze`               | Toggle display freeze                            | `freeze`               |

### Waveform Display
- The waveform is displayed using ASCII characters in an 80x24 grid.
- The trigger level is marked with a horizontal line (`-`).
- The waveform updates in real-time unless frozen.

### Measurements
The oscilloscope calculates and displays the following measurements:
- **Frequency**: Based on zero crossings.
- **Period**: Inverse of frequency.
- **Amplitude**: Peak-to-peak voltage.
- **RMS Voltage**: Root mean square voltage, accounting for DC offset.
- **Duty Cycle**: Percentage of time the signal is above the trigger threshold.
- **Timebase**: Current sampling rate.
- **Trigger Level**: Voltage corresponding to the trigger threshold.

---

## Technical Details

### ADC Configuration
- The ADC prescaler is set to 16 for faster sampling (default is 128).
- ADC resolution: 10 bits (0-1023).
- Reference voltage: 5V (default Arduino Uno).
- Maximum sampling rate: ~10 kSa/s (limited by ADC and delay).

### Triggering System
- **Rising Edge Trigger**: Triggers when the signal crosses the threshold from below.
- **Falling Edge Trigger**: Triggers when the signal crosses the threshold from above.
- **Auto Trigger**: Displays the waveform continuously, regardless of trigger conditions.
- Trigger threshold is adjustable via serial command.

### Sampling and Timebase
- Samples are stored in an 80-element array.
- Timebase (sample delay) is adjustable from 10 µs to 1000 µs.
- Sampling rate is calculated as `1 / sampleDelay` samples per second.

### Measurement Calculations
- **Frequency**: Counts zero crossings and uses sampling rate to estimate frequency.
- **Period**: Calculated as `1000 / frequency` (ms).
- **Amplitude**: Peak-to-peak voltage, scaled by the voltage scale.
- **RMS Voltage**: Calculated using the formula `sqrt(sum((x - avg)^2) / N)`, accounting for DC offset.
- **Duty Cycle**: Percentage of samples above the trigger threshold.

---

## Limitations
- **Sampling Rate**: Limited by Arduino Uno's ADC (~10 kSa/s max).
- **Frequency Range**: Accurate for low-frequency signals (< 1 kHz) due to sampling rate limitations.
- **Voltage Range**: Limited to 0-5V (Arduino Uno ADC range). Use voltage dividers for higher voltages.
- **Display Resolution**: ASCII display is low-resolution and best for basic waveform visualization.
- **Trigger Stability**: May be unstable for noisy signals or high frequencies.

---

## Future Improvements
- **Higher Sampling Rate**: Use direct ADC register access or external ADC for faster sampling.
- **Graphical Display**: Replace ASCII display with an OLED or TFT screen.
- **Advanced Triggering**: Add level triggering, slope detection, and noise rejection.
- **Signal Processing**: Implement FFT for frequency domain analysis.
- **Data Logging**: Save captured waveforms to SD card or PC.
- **Input Protection**: Add hardware protection for overvoltage and negative voltages.

---
