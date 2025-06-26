
# Gatewise - Automated Parking System for BeagleBone Black

A Wiring/Arduino-style hardware control library for the BeagleBone Black platform, designed to support rapid prototyping and deployment of embedded applications. This implementation showcases an automated parking system using GPIO, PWM, UART, and I2C control mechanisms.

---

## 📦 Features

- Arduino-style APIs for easy transition from hobbyist to industrial hardware control
- Real-time GPIO and PWM control using PRU (Programmable Realtime Units)
- UART, SPI, and I2C communication support
- Dynamic overlay configuration at runtime
- Pulse width and frequency capture from input signals
- Modular source code for extension and testing
- GCov-based code coverage reporting

---

## 🛠 Tech Stack

- **Platform**: BeagleBone Black
- **Languages**: C/C++, PRU Assembly
- **Tools**: Make, GCC, GCov
- **Libraries**: beaglebone-universal-io, wiring-like abstraction layer

---

## 📁 Project Structure

```
automated-parking-system/
├── src/                  # Source code for hardware interfaces and core logic
├── am335x_pru_package/   # PRU binaries and helpers
├── coverage_report/      # HTML reports from GCov
├── Makefile.mk           # Build instructions
├── UserPinConfig.h       # Custom pin configuration
└── README.md             # Project documentation
```

---

## 🚀 Getting Started

### 🧰 Prerequisites

- BeagleBone Black with Debian-based OS
- `beaglebone-universal-io` overlay package
- GCC toolchain (`build-essential`)
- Make

### 🔧 Build Instructions

```bash
cd automated-parking-system
make -f Makefile.mk
```

If you encounter missing dependencies, ensure you're using the correct overlay and permissions to access hardware.

---

## 🧪 Running the System

Once built, deploy the binaries or object files directly on the BeagleBone Black. You may need to load the appropriate device tree overlays or give pin permissions depending on your kernel version.

---

## 📊 Code Coverage

HTML reports can be found in:
```
coverage_report/index.html
```

Open this file in a browser to explore line-by-line coverage and function metrics.

---

## 👨‍💻 Authors

- **Nanda Kishore Nallagopu**  
  🔗 [Portfolio](https://nandakishorenallagopu.com)  
  💼 [LinkedIn](https://linkedin.com/in/nandakishorenallagopu)

- **Amlan Choudary**

---

## 📄 License

Specify the project license here (e.g., MIT, GPL, Apache 2.0, etc.).

