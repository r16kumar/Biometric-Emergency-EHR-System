# Cloud-Based Emergency Health Record (EHR) System

## Overview
This repository contains the dual-microcontroller firmware and web dashboard for a secure, multi-biometric emergency medical access node. Designed to eliminate the "Golden Hour" delay in trauma care, the system allows first responders to retrieve critical, encrypted patient medical history (allergies, blood type, conditions) from an unconscious patient via physical biometric authentication.

Through optimized hardware-to-cloud synchronization using Firebase Realtime Database, the system achieves a complete identification and data retrieval cycle in **under 4 seconds**.

---

## 1. Dual-Core Edge Architecture (Firmware)
The physical access node utilizes a distributed processing architecture to ensure rapid biometric matching without locking the main execution loop.

* **Main Processor (ESP32 DevKit V1):** Handles local R307 optical fingerprint enrollment, cryptographic ID generation, and HTTPS GET/PATCH requests to the cloud database.
* **Vision Co-Processor (ESP32-CAM):** Operates as an isolated camera server. 
* **Custom UART Protocol:** To ensure reliable inter-device communication, a custom serial protocol was implemented. Upon successful fingerprint match, the Main ESP32 sends a `CAPTURE_UPLOAD` command. The ESP32-CAM captures a live JPEG frame and streams the binary data in chunks over UART, which is buffered in the Main ESP32's SPIFFS memory before cloud upload.

* [View Main ESP32 Firmware](esp32_main_firmware/)
* [View ESP32-CAM UART/Vision Firmware](esp32_cam_firmware/)

---

## 2. Cloud Database & Web Dashboard
To ensure high availability across hospital networks and emergency vehicles, the hardware is synchronized with a cloud backend. 

* **Firebase Backend:** Patient telemetry is structured in JSON nodes. The hardware utilizes secure HTTPS to fetch and update records dynamically.
* **Netlify Web Dashboard:** A responsive, HTML/JS dashboard utilized by hospital administration to register new patients. Upon hardware fingerprint enrollment, the edge node dynamically generates a QR code linking to a unique registration URL on this dashboard.

* [View Web Dashboard Source Code](web_dashboard/)

---

## 3. Academic Documentation
The complete system architecture, latency analysis, and security protocols have been documented in an academic research format.

* 📄 [Read the Full Academic Draft Paper](research_paper/Draft_Paper.pdf)

---
*Architected and developed by Rohit Kumar and team.*
