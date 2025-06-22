# Dual Actuator Wave-Like Navigator (DAWN)

This repository contains the hardware control code, data analysis scripts, and supplementary software used for the development of the **Dual Actuator Wave-Like Navigator (DAWN)** — a soft robotic system that mimics wave-like locomotion.

## 📁 Repository Structure

```
Daul-Actuator-Wave-Like-Navigator/
│
├── data-and-data-analysis/          # MATLAB scripts and live notebooks for data processing
│   ├── *.mlx                        # Live MATLAB analysis (friction, locomotion, wave studies)
│   ├── *.m                          # Script-based versions for data processing and visualization
│   └── data/                        # Experimental datasets (e.g., pressure, force)
│
├── robot-control-code/             # Arduino/mbed code for the main robot controller
│   ├── main.ino                     # Main controller logic (multithreaded tasks with RTOS)
│   ├── motor_control.ino            # Motor operation and state handling
│   ├── pump_control.ino             # Syringe pump actuation commands
│   └── pressure_sensor.ino          # Sensor interface and data capture
│
├── syringe-pump-code/              # Python and Arduino code for pump-side system
│   ├── pressure_sensors_syringe_switches.ino   # Arduino sketch for sensors and switch control
│   └── syringe_datalloging_switches_pressure.py # Python logger and motion driver for syringe pumps
│
└── .git/                            # Git versioning files (ignore)
```

## 🚀 Getting Started

### Hardware Requirements

- Arduino-compatible microcontroller with RTOS support
- Motor drivers and actuators
- Syringe-based pump system
- Pressure sensors and signal conditioning
- Bluetooth/Wireless modules for command/control

### Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- [MATLAB](https://www.mathworks.com/products/matlab.html)
- Python 3.8+ with packages:
  - `pyserial`, `pandas`, `matplotlib`, `keyboard`, `csv`

## 📊 Data Analysis

MATLAB Live Scripts (`.mlx`) allow rich analysis and visualization of robotic behavior:

- `traveling_wave_analysis.mlx`: Wave pattern effectiveness
- `sand_sledge_analysis.mlx`: Sledge study on granular media
- `simple_locomotion_analysis.mlx`: Basic locomotion profiles and analysis
- `friction_analysis.mlx`: Frictional force modeling
- `pressure_analysis_one_cicle.m`: Single-cycle pressure response

## 🔧 Control Code Highlights

- **Multithreading with RTOS:** Used for concurrent sensor reading, motor control, and pump updates.
- **BLE Communication:** Allows for wireless control and data transmission.
- **Pressure Feedback Loop:** Real-time monitoring of actuation pressure to maintain wave shape.

## 📈 Logging & Experiments

The syringe-pump Python script (`syringe_datalloging_switches_pressure.py`) controls movements and logs data over serial. It allows frequency and speed modulation to experiment with various gait cycles.

## 🧪 Example Usage

1. Upload `main.ino` and related `.ino` files to the robot controller.
2. Upload `pressure_sensors_syringe_switches.ino` to Arduino Mega connected to the syringe system.
3. Run the Python logging script to collect data and drive actuation.
4. Analyze results with MATLAB using the provided `.mlx` scripts.

## 📜 License

This project is currently in development and is shared under the MIT License.
