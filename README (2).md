# 🤖 Integrated Autonomous Buggy Controller

A fully integrated Arduino-based buggy controller that combines line following, obstacle detection, RF gantry detection, and serial supervisory control into a single unified system. The buggy autonomously completes **2 clockwise laps** of a track, stops safely at each gantry, avoids obstacles, and can be overridden at any time via serial commands.

---

## 📁 Project Files

| File | Description |
|------|-------------|
| `buggy_integrated.ino` | ✅ **Main file** — fully integrated controller |
| `code1_basic_motor_control.ino` | Basic motor movement demo (all directions) |
| `code2_ir_line_following.ino` | IR sensor-based line following |
| `code3_ir_ultrasonic.ino` | Line following + ultrasonic obstacle detection |
| `code4_ir_ultrasonic_gantry.ino` | Line following + ultrasonic + RF gantry detection |
| `code5_serial_supervisory.ino` | Full manual serial/Bluetooth supervisory control |

---

## 🔌 Hardware Requirements

### Components
- Arduino Uno / Nano (or compatible)
- L298N Motor Driver Module (or equivalent dual H-bridge)
- 2× DC motors (left and right drive wheels)
- 2× IR sensors (line following)
- HC-SR04 Ultrasonic Sensor (obstacle detection)
- RF receiver module on D4 (gantry detection)
- Optional: HC-05 / HC-06 Bluetooth module (for wireless serial control)
- Power supply (7–12V battery pack)

### Pin Mapping

| Component | Pin |
|-----------|-----|
| Motor A — IN1 | D5 |
| Motor A — IN2 | D6 |
| Motor B — IN3 | D7 |
| Motor B — IN4 | D8 |
| IR Sensor Left | A0 |
| IR Sensor Right | A1 |
| Ultrasonic TRIG | D13 |
| Ultrasonic ECHO | D12 |
| RF / Gantry pulse | D4 |

---

## ⚙️ How It Works

### Autonomous Mode (Default)
On power-up the buggy enters **AUTO mode** and begins the mission:

1. **Line Following** — dual IR sensors (A0, A1) keep the buggy on the track
2. **Obstacle Detection** — HC-SR04 checks for objects closer than 20 cm; if detected, the buggy stops for 2 seconds and retries
3. **Gantry Detection** — RF pulse widths on D4 identify which of the 3 gantries has been crossed; buggy pauses and prints a status report at each one
4. **Lap Counting** — every time Gantry 3 is crossed, a lap is recorded; after 2 laps the buggy parks safely

### IR Sensor Logic

| IR Left (A0) | IR Right (A1) | Action |
|:---:|:---:|--------|
| 1 | 1 | Forward (both on line) |
| 0 | 1 | Turn Left (drifted right) |
| 1 | 0 | Turn Right (drifted left) |
| 0 | 0 | Forward (best guess) |

### Gantry RF Pulse Windows

| Gantry | Pulse Width |
|--------|-------------|
| Gantry 1 | 1500 – 2000 µs |
| Gantry 2 | 2500 – 3000 µs |
| Gantry 3 | 500 – 1000 µs |

---

## 🎮 Serial Supervisory Commands

Open **Serial Monitor at 9600 baud** (or connect via Bluetooth) and send single-character commands:

| Command | Action |
|---------|--------|
| `F` | Forward |
| `B` | Backward |
| `L` | Turn Left |
| `R` | Turn Right |
| `C` | Pivot Clockwise |
| `A` | Pivot Anti-clockwise |
| `S` | Stop motors |
| `P` | Safe Park (works in both modes) |
| `M` | Toggle AUTO ↔ MANUAL mode |
| `?` | Print current buggy status |

> **Note:** In AUTO mode, only `P` and `M` are accepted. All movement commands become active after switching to MANUAL mode with `M`.

---

## 📊 Status Report

At every gantry crossing, obstacle event, park, and startup, the buggy prints a status report to Serial:

```
╔══════════════════════════════╗
║      BUGGY STATUS REPORT     ║
╠══════════════════════════════╣
║ Event      : GANTRY 1
║ Lap        : 1 / 2
║ Gantry #   : 1
║ IR Left    : ON LINE
║ IR Right   : ON LINE
║ Distance   : 45 cm
║ Mode       : AUTO
║ Parked     : NO
╚══════════════════════════════╝
```

---

## 🚀 Getting Started

1. Wire the components according to the pin mapping table above
2. Open `buggy_integrated.ino` in the Arduino IDE
3. Select your board and COM port under **Tools**
4. Click **Upload**
5. Open **Serial Monitor** at **9600 baud**
6. The buggy will print a startup message and begin the mission after a 2-second delay
7. Send `?` at any time to check status, or `M` to take manual control

---

## 🔧 Adjustable Constants

Inside `buggy_integrated.ino`, these values can be tuned at the top of the file:

| Constant | Default | Description |
|----------|---------|-------------|
| `OBSTACLE_DIST_CM` | 20 | Stop distance threshold (cm) |
| `TOTAL_LAPS` | 2 | Number of laps before auto-park |
| `GANTRY_COUNT` | 3 | Number of gantries per lap |
| `GANTRY_PAUSE_MS` | 1000 | Pause duration at each gantry (ms) |
| `OBSTACLE_PAUSE_MS` | 2000 | Wait time when obstacle detected (ms) |

---

## 🧩 Code Evolution (Individual Files)

The individual code files represent the progressive development stages of the project:

```
Code 1  →  Basic motor control (all directions)
  ↓
Code 2  →  + IR line following
  ↓
Code 3  →  + Ultrasonic obstacle avoidance
  ↓
Code 4  →  + RF gantry detection & Serial printing
  ↓
Code 5  →  + Serial supervisory control
  ↓
Integrated  →  All features unified, 2-lap mission, safe parking
```

---

## ⚠️ Notes & Known Issues

- The original Code 4 referenced an undeclared variable `flag` — this is resolved in the integrated file using the `parked` and `mode` state variables instead
- Ensure IR sensors are calibrated for the specific track surface (black line on white background is recommended)
- If the RF pulse window does not match your transmitter, adjust the `GANTRY1_LOW / HIGH` constants accordingly
- The ultrasonic sensor has a 30 ms timeout in `pulseIn()` — if no echo is received, the distance defaults to 999 cm (treated as clear)

---

## 📝 License

This project was developed as an educational embedded systems exercise. Free to use and modify.
