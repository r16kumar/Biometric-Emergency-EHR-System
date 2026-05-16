# Cloud-Based Emergency Health Record Access System 🏥☁️

🎥 **Watch the Live System Demonstration on YouTube:** [(22) Cloud-Based Emergency Health Record System | ESP32, Firebase & Multi-Biometric Auth - YouTube](https://www.youtube.com/watch?v=DwHUV-2MV_s)

This repository contains the hardware firmware, web UI components, and documentation for a portable, cloud-based Emergency Health Record (EHR) access system. Built using an ESP32 DevKit V1 and ESP32-CAM, this IoT solution utilizes multi-biometric authentication (fingerprint and facial capture) to securely and rapidly identify unconscious or unresponsive patients during medical emergencies.  

Once a patient is authenticated, the system retrieves critical health data—such as blood type, known allergies, and medical conditions—from a Firebase Realtime Database. The information is instantly displayed locally on an OLED screen and synchronized to a Netlify-hosted web dashboard, ensuring that first responders have immediate access to life-saving information without relying on the patient's personal devices.

---

## ✨ Key Features
* **Multi-Biometric Security:** Combines R307 fingerprint matching with ESP32-CAM real-time face capture for robust authentication.
* **Cloud Integration:** Real-time data synchronization using Firebase Realtime Database.
* **Instant Visualization:** Critical data is displayed locally on a 1.3" OLED screen.
* **QR-Based Registration:** Dynamic QR code generation for frictionless patient onboarding via a Netlify-hosted web dashboard.

---

## ⚙️ Hardware Components
* **Main Controller:** ESP32 DevKit V1 (30 pins)
* **Camera Module:** ESP32-CAM (AI-Thinker) with OV2640 Sensor
* **Biometric Sensor:** R307 Optical Fingerprint Module
* **Display:** 1.3" OLED Display (SH1106 Driver, I2C)
* **Input:** 4x4 Membrane Keypad
* **Power:** 5V USB Supply

---

## 🔌 Pin Configuration

### ESP32 DevKit V1 Connections
| Component | Pin on ESP32 DevKit | Function |
| :--- | :--- | :--- |
| **R307 Fingerprint TX** | GPIO 16 (RX2) | Serial Data In |
| **R307 Fingerprint RX** | GPIO 17 (TX2) | Serial Data Out |
| **ESP32-CAM TX** | GPIO 4 | UART Receive (from Cam) |
| **ESP32-CAM RX** | GPIO 15 | UART Transmit (to Cam) |
| **OLED SH1106 SDA** | GPIO 21 | I2C Data |
| **OLED SH1106 SCL** | GPIO 22 | I2C Clock |
| **Keypad Rows (1-4)** | GPIO 32, 33, 25, 26 | Row Inputs |
| **Keypad Cols (1-4)** | GPIO 27, 14, 13, 23 | Column Outputs |

*(Note: The ESP32-CAM requires a stable 5V supply to boot properly.)*

---

## 💻 Software & Tech Stack
* **Firmware:** C++ (Arduino IDE) for ESP32 and ESP32-CAM.
* **Database:** Firebase Realtime Database (JSON tree structure).
* **Web Dashboard:** HTML/CSS/JS, hosted on Netlify.
* **Key Libraries:** `U8g2lib`, `Keypad`, `Adafruit_Fingerprint`, `WiFi`, `HTTPClient`, `ArduinoJson`, `qrcodem`, `SPIFFS`.

---

## 🛠️ Setup Instructions
1. **Hardware Assembly:** Wire the components according to the Pin Configuration table above.
2. **Firebase Setup:** * Create a Firebase Realtime Database.
   * Set up your JSON structure (`patients/<ID>` and `fingerMap/<slot>`).
   * Update the `databaseURL` in the main ESP32 code.
3. **Web Dashboard:** Deploy the front-end dashboard code to Netlify and update the `webBaseURL` in your ESP32 code.
4. **Firmware Upload:**
   * Flash `main_controller.ino` to the ESP32 DevKit V1.
   * Flash `camera_module.ino` to the ESP32-CAM.
   * Update the WiFi `ssid` and `password` credentials in both scripts before uploading.

---

## 👥 Contributors
* Yash Chauhan (06911502822)
* Ridyansh Thakur (07411502822)
* Garvit Bharadwaj (08711502822)
* Rohit Kumar (09611502822)

**Guided by:** Dr. Mihika, Assistant Professor  
Bharati Vidyapeeth's College of Engineering, New Delhi
