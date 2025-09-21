# Homemade 4×HDMI KVM Controller

This project is a DIY KVM‑like controller that manages **four HDMI 3‑in/1‑out switch boards** using a **WeMos D1 Mini (ESP8266)** and a **PCF8575 I²C I/O expander**. The ESP8266 provides remote control and monitoring of HDMI input selection through MQTT/HTTP, making it possible to integrate with smart home systems like Home Assistant.

---

## ✨ Features

* Control **4× HDMI 3‑in/1‑out switches** over Wi‑Fi.
* Simulate button presses to switch HDMI inputs.
* Monitor switch LED states to confirm active inputs.
* Expose MQTT topics and optional HTTP API for integration.
* Debug and control locally via **Serial commands**.
* Compact and low‑cost design using off‑the‑shelf modules.

---

## 🛠 Hardware

* **WeMos D1 Mini (ESP8266, Micro‑USB)** – main controller.
* **PCF8575 I²C → 16 I/O module** – extends GPIOs for buttons/LEDs.
* **4× HDMI Switch Boards** – 3‑in/1‑out, 4K/1080p capable.
* **NPN transistors (2N3904 or similar)** + **10 kΩ base resistors** – safe button simulation.
* **Resistor dividers** – scale LED voltages to 3.3 V logic.
* **Pull‑ups (4.7–10 kΩ)** – I²C lines.
* **Perfboard + wiring harnesses** – connections.
* **USB power distribution (5 V)** – shared across all modules.

---

## 🔌 Connections

* ESP8266 ↔ PCF8575: I²C bus (SDA/SCL + pull‑ups).
* PCF8575 → HDMI Switches:

  * Outputs → transistor circuits → button pads.
  * Inputs ← resistor dividers ← LED signals.
* Shared ground across all modules.

---

## 📡 Interfaces

### MQTT Topics

* **Commands:**

  * `kvm/cmd/route` → `{ "output": 1, "input": 2 }`
  * `kvm/cmd/press/{id}` → `{ "ms": 120 }`
* **State:**

  * `kvm/state/output/{id}` → `{ "input": 2, "leds": [0,1,0], "ts": ... }`

### HTTP API (optional)

* `GET /api/state` → JSON of all outputs.
* `POST /api/route` → body: `{ "output": 1, "input": 2 }`.
* `POST /api/press/{id}` → body: `{ "ms": 120 }`.

---

## ⌨️ Serial Command Reference

When connected via USB to the ESP8266 (baud **115200**), you can issue commands directly:

| Command | Action                                                  |
| ------- | ------------------------------------------------------- |
| `1`     | Pulse **P3** high → low (simulate button press #1).     |
| `2`     | Pulse **P7** high → low (simulate button press #2).     |
| `3`     | Pulse **P11** high → low (simulate button press #3).    |
| `4`     | Pulse **P15** high → low (simulate button press #4).    |
| `9`     | Read and print all PCF8575 input pin values (P15 → P0). |

**Example Output (Command `9`):**

```
1 - 0 - 1 - 0 - 1 - 0 - 0 - 1 - 1 - 0 - 0 - 1 - 1 - 0 - 0 - 1
```

Each number corresponds to the state of pins P15 down to P0.

---

## 📐 System Diagram (GitHub‑compatible Mermaid)

> GitHub renders Mermaid fenced code blocks. Use **`graph TD`** for maximum compatibility.

```mermaid
graph TD
  %% Control Layer
  ESP[ESP8266 / WeMos D1 Mini]
  PCF[PCF8575 (I²C → 16 I/O)]
  ESP -- I²C --> PCF

  %% HDMI Switch Boards
  SW1[HDMI Switch #1\n3-in → 1-out]
  SW2[HDMI Switch #2\n3-in → 1-out]
  SW3[HDMI Switch #3\n3-in → 1-out]
  SW4[HDMI Switch #4\n3-in → 1-out]

  %% Wiring
  PCF ---|BTN/LED wires| SW1
  PCF ---|BTN/LED wires| SW2
  PCF ---|BTN/LED wires| SW3
  PCF ---|BTN/LED wires| SW4

  %% Network Interfaces
  MQTT[(MQTT Broker)]
  HTTP[HTTP REST]
  HA[Home Assistant]

  ESP -. Wi‑Fi .-> MQTT
  ESP -. Wi‑Fi .-> HTTP
  ESP -. Wi‑Fi .-> HA
```

### ASCII Fallback (if Mermaid is disabled)

```
+------------------------------+         +-------------------+
| ESP8266 (WeMos D1 Mini)      |  I2C    |  PCF8575          |
|  Wi‑Fi + logic + APIs        +---------+  16x GPIO (I²C)   |
+------------------------------+         +---------+---------+
                                                   |
                             BTN/LED wires to 4x HDMI 3-in/1-out switches
                                                   |
      +-----------------+  +-----------------+  +-----------------+  +-----------------+
      | HDMI Switch #1  |  | HDMI Switch #2  |  | HDMI Switch #3  |  | HDMI Switch #4  |
      | 3-in → 1-out    |  | 3-in → 1-out    |  | 3-in → 1-out    |  | 3-in → 1-out    |
      +-----------------+  +-----------------+  +-----------------+  +-----------------+

Wi‑Fi → MQTT / HTTP / Home Assistant
```

---

## 🚀 Getting Started

1. Assemble hardware on perfboard (ESP8266, PCF8575, wiring to switches).
2. Flash ESP8266 with Arduino/PlatformIO firmware.
3. Configure Wi‑Fi and MQTT credentials.
4. Verify I²C communication (PCF8575 address, SDA/SCL pull‑ups).
5. Map PCF8575 pins to button/LED lines in firmware.
6. Test button press simulation and LED reads on one board.
7. Scale up to all 4 HDMI switches.
8. Integrate with Home Assistant (MQTT discovery).

---

## 📋 To‑Do / Next Steps

* Add per‑input direct select (instead of cycling button).
* Add local OLED display for quick status view.
* Optional isolation with optocouplers for noisy HDMI boards.
* Improve enclosure / cable management.

---

## 📄 License

Open‑source under MIT License. Feel free to adapt and improve!
