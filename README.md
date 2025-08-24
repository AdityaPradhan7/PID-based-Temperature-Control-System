# PID-based Temperature Control System (FreeRTOS Simulation)

This project demonstrates a **real-time PID (Proportional–Integral–Derivative) temperature control simulation** built using **C** and **FreeRTOS**.  
It models a simplified cooling system where a **cooling unit** adjusts its power to maintain a desired setpoint temperature despite environmental heating and sensor noise.

---

## 🚀 Features

- Real-time temperature control simulation using **FreeRTOS tasks, queues, and timers**.
- **PID controller** with anti-windup logic for stable control.
- Simulated environment:
  - Natural heating effect
  - Cooling power effect
  - Random environmental disturbances
  - Sensor noise
- Configurable PID parameters (`Kp`, `Ki`, `Kd`) and setpoint.

---

## 🛠️ Tools & Technologies

- **C** (developed & tested in Visual Studio)
- **FreeRTOS** (for tasks, queues, timers)
- **RTOS concepts**: task scheduling, software timers, message queues

---

## 📂 Project Structure

├── `main.c` - Core simulation (PID logic + FreeRTOS tasks/timers)  
├── `FreeRTOSConfig.h` - FreeRTOS configuration  
├── `README.md` - Project documentation  
└── (other FreeRTOS source files as needed)

---

## ⚙️ Setup & Run

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/pid-temperature-control.git
   cd pid-temperature-control
   ```
2. Open the project in Visual Studio (or any C IDE supporting FreeRTOS).

3. Build and run [Debug & x86]. The console will show simulated temperature readings and PID-controlled cooling power values every second.

## 📊 Example Output

```FreeRTOS Temperature Monitoring & Control Simulation (PID Version)
Temperature received: 27 C - Cooling Power: 9.9%
Temperature received: 30 C - Cooling Power: 21.3%
Temperature received: 32 C - Cooling Power: 38.6%
Temperature received: 28 C - Cooling Power: 15.9%
...
```

🧑‍💻 Developer Notes
🔹 System Parameters

- Setpoint (Target Temperature): 25 °C

- Cooling Power Authority: 0–100% (scaled effect on temperature drop)

- Environmental Heating Rate: +0.2 °C/sec (baseline heat gain)

- Cooling Rate: up to -0.6 °C/sec at 100% cooling power

- Sensor Noise: ±0.5 °C simulated disturbance

## ASCII Fallback diagram

```
   +-----------+       +-------------+       +----------------+       +------------------+
   | Setpoint  | ----> |    PID      | ----> | Cooling Power  | ----> |  Environment &   |
   |  (25 °C)  |       | Controller  |       |   (0–100 %)    |       |  Temperature     |
   +-----------+       +-------------+       +----------------+       +------------------+
                                                                                 |
                                                                                 v
                                                                          +--------------+
                                                                          |  Sensor      |
                                                                          |  Reading     |
                                                                          +--------------+
                                                                                 |
                                                                                 v
                                                                           (Feedback loop)
```                                                                           

## System Flow (FreeRTOS)

        +------------------+
        |   Temp Timer     |
        | (1s periodic)    |
        +--------+---------+
                 |
                 v
        +------------------+      +------------------+
        | Temperature Task | ---> |   Control Task   |
        |  (Sensor model)  |      |  (PID + Cooling) |
        +------------------+      +------------------+
                                           |
                                           v
                                   Cooling Power Output

## Developer Notes

- **Setpoint Temperature:** 25 °C
- **Sensor Noise:** ±0.5 °C (random variation added to simulate real-world sensor)
- **Environmental Heat Gain:** Temperature naturally drifts upward by ~+0.1 °C per cycle (simulates external heat sources).
- **Cooling Effect:** Cooling reduces temperature proportionally to power output (0% = no cooling, 100% = maximum cooling).
- **PID Controller Gains (example tuning):**
  - Kp = 2.0 (Proportional gain, reacts to current error)
  - Ki = 0.5 (Integral gain, corrects accumulated error over time)
  - Kd = 1.0 (Derivative gain, reacts to rate of temperature change)
- **Anti-Windup:** Prevents integral term from growing when cooler is already maxed out (0% or 100% power). This avoids overshoot and slow recovery.
- **Feedback Loop Behavior:**
  1. Measure temperature
  2. Compare against setpoint → compute error
  3. PID adjusts cooler power (0–100%)
  4. Cooling + environment update temperature
  5. Loop repeats

## 🧮 PID Controller Math

The control signal is calculated as: (continuous form)

`u(t) = Kp × e(t) + Ki × ∫ e(t) dt + Kd × (de(t)/dt)`

OR

`u(t) = Kp * e(t) + Ki * ∑ e(t) + Kd * (e(t) - e(t-1))`

Where:

- e(t) = measuredTemp - setpoint (temperature error)
- Kp = 3.0 (Proportional gain)
- Ki = 0.1 (Integral gain)
- Kd = 1.5 (Derivative gain)
- Output u(t) is clamped between 0–100% (cooling power).

## How PID Works in This Project

The PID controller adjusts cooling power to keep the temperature close to the setpoint (25 °C). Here’s how each part contributes:

---

### Proportional (P)

- This is the "right now" term.
- It reacts directly to the current error (difference between measured temperature and setpoint).
- Bigger error → stronger cooling.
- **Equation:**  
  P = Kp × error

- **Positive vs Negative Error:**
  - If measured temperature is **above setpoint** → error is **positive** → proportional term pushes cooler ON harder.
  - If measured temperature is **below setpoint** → error is **negative** → proportional term lowers cooling power (or even turns cooler fully OFF).
  - **Example:**
  - If the room is 30 °C (5 °C above setpoint), proportional term applies strong cooling immediately.
  - If the room drops to 23 °C (2 °C below setpoint), proportional term reduces cooling to avoid making it even colder.

---

### Integral (I)

- This is the "memory" of the system.
- It’s the sum of past errors over time.
- If the system stays slightly above 25 °C for a long time, the integral keeps adding up and slowly increases cooling power to eliminate that bias.
- **Equation (discrete form used in code):**  
   I = I + Ki × error
- **Positive vs Negative Error:**
  - If error stays **positive** (too hot for too long), the integral **keeps increasing**, forcing more cooling.
  - If error stays **negative** (too cold for too long), the integral **decreases**, reducing cooling.
  - **Example:**
  - If temp is stuck at 26 °C for a while, the integral gradually ramps up cooling until it hits 25 °C.
  - If temp is stuck at 23 °C, the integral slowly reduces cooling until it stabilizes back at 25 °C.

---

### Derivative (D)

- This is the "look ahead" term.
- It reacts to how fast the error is changing.
- If the temperature is rising quickly, D adds extra cooling before things get out of hand.
- **Equation (discrete form in code):**

  D = Kd × (error − lastError)

- **Positive vs Negative Error Change:**
  - If error is **increasing rapidly** (temperature rising), derivative is **positive** → more cooling is applied quickly.
  - If error is **decreasing rapidly** (temperature falling), derivative is **negative** → cooling is reduced to avoid overshoot.
  - **Example:**
  - If temp jumps suddenly from 25 °C → 28 °C, derivative provides an early braking force (extra cooling).
  - If temp drops fast from 25 °C → 22 °C, derivative cuts cooling so it doesn’t overshoot too cold.

---

### Anti-Windup Logic

- Without this, the integral could keep growing even when cooling is already maxed out at **100%**.
- That would cause overshoot (temperature dropping too low once the cooler finally catches up).
- With **anti-windup**, the integral **pauses** whenever output is clamped at 0% or 100%, keeping the system stable and avoiding wild swings.

---

## 📘 Learning Takeaways

- Real-time systems: using FreeRTOS tasks and queues for periodic control.
- PID control theory applied to embedded systems.
- Handling sensor noise and environmental disturbances in simulations.
- Anti-windup strategies for integral stability.

👉 In short:

- **P** fixes the error you see right now.
- **I** which fixes the errors that piled up in the past. It is the sum of the past errors.
- **D** predicts and prevents errors that are about to happen.
- **Anti-windup** keeps everything balanced when the cooler is at its limits.
