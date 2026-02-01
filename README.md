# Digital Weigh Scale (Arduino + Load Cell + HX711)

**Course:** Electrical Instrumentation and Measurement (ECE2024)  
**Instructor:** Dr. Vo Bich Hien  
**Authors:** Nguyen Hoang Long (10223088), Vu Hoang Minh (102240574)

A low-cost digital weighing scale built around a strain-gauge **load cell** and the **HX711 24-bit ADC + PGA**, with weight shown on a **4-digit 7-segment LED module** (74HC595 shift-register driver) and streamed to Serial in real time.

<p align="center">
  <img src="Media/Front.jpg" width="850" alt="Front view of the scale" />
</p>

---

## Contents

- [Project overview](#project-overview)
- [Demo](#demo)
- [Hardware](#hardware)
- [Wiring](#wiring)
- [Build notes (mechanical)](#build-notes-mechanical)
- [Firmware](#firmware)
- [Calibration & tare](#calibration--tare)
- [Testing & results](#testing--results)
- [Troubleshooting](#troubleshooting)
- [Limitations & improvements](#limitations--improvements)
- [Repository structure](#repository-structure)
- [License](#license)
- [Media gallery](#media-gallery)

---

## Project overview

### Functional blocks


```
Force / Weight
|
▼
Load Cell (Wheatstone Bridge)
| (mV/V differential signal)
▼
HX711 24-bit ADC + PGA
| (digital data)
▼
Arduino Uno (processing, tare, calibration)
|------------------> Serial Monitor (g=<value>, commands)
▼
4-Digit 7-Segment Display (74HC595 shift registers)
```


### Features

- Live weight readout in **grams**.
- 4-digit display range **0 → 9999 g** (values are clamped).
- Serial output every ~100 ms: `g=<value>`.
- Simple serial commands:
  - `t` → tare (zero)
  - `c <grams>` → calibrate using a known mass

---

## Demo

- **Final build (with enclosure):** [Media/Testing.mp4](Media/Testing.mp4)
- **Prototype (no enclosure):** [Media/Testing 2.mp4](Media/Testing%202.mp4)

<p align="center">
  <img src="Media/Model.jpg" width="850" alt="Overall prototype" />
  <br/>
  <em>Overall assembled prototype (platform + enclosure + display).</em>
</p>

---

## Hardware

### Bill of Materials (BOM)

- Arduino Uno (or compatible)
- Bar-type strain gauge **load cell** (project platform label shows **10 kg**)
- **HX711** load-cell amplifier/ADC module
- 4-digit 7-segment LED module (74HC595 driver)
- Platform plates (acrylic), standoffs/spacers, fasteners
- Jumper wires, enclosure material (cardboard in this build)

<p align="center">
  <img src="Media/Internal.jpg" width="850" alt="Internal wiring" />
  <br/>
  <em>Internal layout: Arduino + HX711 + wiring routed to the platform and display.</em>
</p>

---

## Wiring

### Pin mapping (matches the firmware)

From [`Source Code/digital_weigh_scale.ino`](Source%20Code/digital_weigh_scale.ino):

- `HX711_ADC LoadCell(A0, A1);`  → **DOUT/DT = A0**, **SCK = A1**
- `Led4digit74HC595 disp(A5, A4, A3);` → **SCLK = A5**, **RCLK = A4**, **DIO = A3**

| Module | Signal | Arduino Uno |
|---|---|---:|
| HX711 | DT (DOUT) | A0 |
| HX711 | SCK | A1 |
| 4-digit display | DIO | A3 |
| 4-digit display | RCLK (latch) | A4 |
| 4-digit display | SCLK (shift clk) | A5 |
| HX711 + display | VCC | 5V |
| HX711 + display | GND | GND |

### Schematic

<p align="center">
  <img src="Media/Schematic.png" width="900" alt="Schematic" />
  <br/>
  <em>Arduino Uno ↔ HX711 ↔ load cell, plus 4-digit display (74HC595).</em>
</p>

### Load cell wiring notes

Load cell wire colors vary. A common convention is:

- **Red = E+**, **Black = E−** (excitation)
- **Green = A+**, **White = A−** (signal)

Always verify against your load cell’s label or the HX711 board markings.

<p align="center">
  <img src="Media/Loadcell_2.jpg" width="850" alt="Load cell and HX711 inside" />
  <br/>
  <em>Load cell + HX711 wiring inside the enclosure (close-up).</em>
</p>

---

## Build notes (mechanical)

Bar load cells measure bending. The platform must apply force so the sensing region flexes consistently.

**Practical tips used in this build**

- Keep the base plate rigid; avoid wobble.
- Apply the load near the platform center.
- Avoid preloading the beam by over-tightening one side.
- Route sensor wires so they are not tugging the load cell.

Reference layouts:

<p align="center">
  <img src="Media/load-cell-scale-arduino.jpg" width="850" alt="Real mounting between plates" />
  <br/>
  <em>Example mounting between top/bottom plates.</em>
</p>

<p align="center">
  <img src="Media/load-cell-setup-scale.jpg" width="850" alt="Recommended load cell setup" />
  <br/>
  <em>Recommended spacer/standoff arrangement to let the beam flex.</em>
</p>

Additional views of the platform/load cell used:

<p align="center">
  <img src="Media/Loadcell_1.jpg" width="850" alt="Side view of platform and load cell" />
  <br/>
  <em>Side view: acrylic platform plates and bar-type load cell placement.</em>
</p>

---

## Firmware

### Dependencies

Two Arduino libraries are included as ZIPs in `Libraries/`:

- `HX711_ADC-master.zip`
- `Led4digit74HC595-master.zip`

**Arduino IDE install:** *Sketch → Include Library → Add .ZIP Library…* (add both ZIPs), then open and upload:

- `Source Code/digital_weigh_scale.ino`

### Runtime behavior

- The display is refreshed continuously (`disp.loopShow()`), while the HX711 is updated (`LoadCell.update()`).
- Every ~100 ms the firmware:
  - reads grams (`LoadCell.getData()`),
  - rounds to an integer,
  - clamps to **0…9999**,
  - prints `g=<grams>` over Serial,
  - updates the 4-digit display.

<p align="center">
  <img src="Media/running.jpg" width="850" alt="System running" />
  <br/>
  <em>System running (Arduino powered, display active).</em>
</p>

---

## Calibration & tare

Open **Serial Monitor** at **115200 baud**.

### Tare

- Command: `t`
- Effect: sets the current platform load as zero.

### Calibration

- Command: `c <grams>` (example: `c 205`)
- Steps:
  1. Tare the empty platform (`t`).
  2. Place a known mass.
  3. Send `c <known_mass_in_grams>`.

The sketch updates the calibration factor using a ratio:

$$
calFactor_{new} = calFactor_{old} \times \frac{measured}{real}
$$

> The updated factor is applied immediately, but it is **not saved permanently**. If you want it to persist, copy the printed value into `calFactor` in the sketch (or store it in EEPROM as an upgrade).

### Display range / clamping

Because the display has 4 digits:

- Values `< 0` display as `0`
- Values `> 9999` display as `9999`

<p align="center">
  <img src="Media/Number.jpg" width="850" alt="Display clamped to 9999" />
  <br/>
  <em>Example: values above the range are clamped to 9999 g.</em>
</p>

---

## Testing & results

Two test videos are included in the repository:

- [Testing.mp4](Media/Testing.mp4) — final product with enclosure
- [Testing 2.mp4](Media/Testing%202.mp4) — barebone prototype

---

## Troubleshooting

- **Reading is negative or unstable**
  - Re-check load cell wiring order on HX711 (A+/A− swapped can invert sign).
  - Ensure the load cell is mounted so it can flex (no binding).
  - Shorten sensor wires or twist pairs to reduce noise.

- **Always shows 0 or doesn’t change**
  - Confirm HX711 pins: DT → A0, SCK → A1.
  - Confirm HX711 has power (VCC/GND).
  - Make sure the load cell is actually stressed (platform isn’t bottoming out).

- **Display not updating / flickering**
  - Confirm display pins: DIO → A3, RCLK → A4, SCLK → A5.
  - Check that `disp.loopShow()` is called frequently (it is in `loop()`).

---

## Limitations & improvements

**Current limitations**

- Calibration factor is not saved across resets.
- Display limited to **0–9999 g**.
- Mechanical rigidity and vibration strongly affect repeatability.

**Suggested improvements**

- Store calibration and tare offset in **EEPROM**.
- Add digital filtering (moving average / median) and a stability indicator.
- Add push buttons for **tare** and **calibrate** (standalone operation).
- Improve enclosure/platform rigidity for better repeatability.

---

## Repository structure

- `Source Code/`
  - `digital_weigh_scale.ino` — Arduino sketch
- `Libraries/`
  - `HX711_ADC-master.zip`
  - `Led4digit74HC595-master.zip`
- `Media/`
  - Photos + schematic + test videos

---

## License

MIT — see [LICENSE](LICENSE).

---

## Media gallery

All media assets referenced in this README live in `Media/`.

- Prototype / build photos:
  - `Media/Model.jpg`
  - `Media/Front.jpg`
  - `Media/Internal.jpg`
  - `Media/Loadcell_1.jpg`
  - `Media/Loadcell_2.jpg`
  - `Media/running.jpg`
  - `Media/Number.jpg`

- Wiring + mechanical references:
  - `Media/Schematic.png`
  - `Media/load-cell-scale-arduino.jpg`
  - `Media/load-cell-setup-scale.jpg`

- Videos:
  - `Media/Testing.mp4`
  - `Media/Testing 2.mp4`
