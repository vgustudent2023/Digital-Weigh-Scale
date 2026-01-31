# Digital Weigh Scale (Arduino + HX711 + 4-Digit 7-Segment)

A simple **digital weighing scale** built with an Arduino, a **load cell + HX711 24-bit ADC**, and a **4-digit 7-segment LED display** driven by **74HC595** shift registers.  
It reads weight in **grams**, prints it to **Serial**, and shows it on the 4-digit display.

---

## Features

- Weight readout in **grams** (rounded to an integer).
- 4-digit display output (**0 → 9999 g**).
- Serial output format: `g=<value>`.
- Serial commands:
  - `t` → tare (zero the scale)
  - `c <grams>` → calibrate using a known mass

---

## Hardware

- Arduino (UNO / Nano / compatible)
- Load cell (e.g., 1–5 kg) + **HX711** module
- 4-digit 7-segment display module using **74HC595** (common 3-wire modules)
- Wires + stable mounting platform

---

## Wiring

### HX711 → Arduino

| HX711 | Arduino |
|------:|:-------|
| DOUT  | A0 |
| SCK   | A1 |

> Load cell connects to the HX711 module (E+, E-, A+, A-). Follow your module/load cell labels.

### 4-Digit 7-Segment (74HC595) → Arduino

The sketch uses:
`Led4digit74HC595 disp(A5, A4, A3);  // SCLK, RCLK, DIO`

| Display | Arduino | Meaning |
|--------:|:-------:|:--------|
| SCLK    | A5      | Shift clock |
| RCLK    | A4      | Latch clock |
| DIO     | A3      | Data |

---

## Libraries

This project uses:

- **HX711_ADC** (olkal)
- **Led4digit74HC595** (LuBossCzech)

### Install (Arduino IDE)

**Option A — Library Manager**
1. **Tools → Manage Libraries…**
2. Search & install:
   - `HX711_ADC`
   - `Led4digit74HC595`

**Option B — ZIP install**
1. **Sketch → Include Library → Add .ZIP Library…**
2. Add the provided ZIPs:
   - `HX711_ADC-master.zip`
   - `Led4digit74HC595-master.zip`

---

## Build & Upload

1. Open `digital_weigh_scale.ino` in Arduino IDE.
2. Select board + port.
3. Upload.
4. Open **Serial Monitor** at **115200 baud**.

---

## Usage

### Startup behavior
- Initializes the display.
- Initializes HX711 and performs an initial tare.
- Uses an initial calibration factor in code:
  - `calFactor = 214.4f` (your value may differ after calibration)

### Live output
- Updates readings continuously.
- Every ~100 ms:
  - Reads grams, rounds to integer
  - Clamps to **0..9999**
  - Displays the value
  - Prints `g=<value>` to Serial

---

## Calibration

Calibration is done via **Serial Monitor**.

### 1) Tare (zero)
1. Remove everything from the scale.
2. Send:
   - `t`

### 2) Calibrate with a known mass
1. Place a known weight on the scale (example: **205 g**).
2. Send:
   - `c 205`

The sketch computes a new calibration factor based on the measured vs real value and prints it to Serial.

### 3) Save the calibration factor permanently (recommended)
The sketch applies the new factor at runtime, but it **won’t persist after reset** unless you save it:
1. Copy the printed `New calFactor=...`
2. Replace the line in code:
   - `float calFactor = <your_value>;`
3. Upload again.

---

## Display Limits

- Shows only **0 to 9999 g**
- Negative values are forced to **0**
- Values above 9999 are forced to **9999**

---

## Troubleshooting

### Readings are noisy / unstable
- Shorten and twist load cell wires if possible.
- Improve mechanical mounting (no wobble).
- Ensure stable power (USB noise can affect readings).

### Always reads 0 or doesn’t change
- Check HX711 wiring: DOUT/SCK must match **A0/A1**.
- Recheck load cell wiring to HX711 (E+/E-/A+/A-).

### Display is blank / wrong
- Confirm display wiring: **A5/A4/A3** (SCLK/RCLK/DIO).
- Verify your module is compatible with `Led4digit74HC595`.

---

## Serial Command Reference

- `t` — tare / zero the scale
- `c <grams>` — calibrate with a known mass  
  Example: `c 205`

---

## License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.  
See the `LICENSE` file for details.
