# ESP 32 C3 Firmware Binaries

This folder contains compiled binaries for Over-The-Air (wireless) updates and USB flashing.

---

## 1. Web OTA Update (Wireless via Browser)
- **File**: `firmware.bin`
- **Usage**:
  1. Connect to Wi-Fi `ESP 32 C3`.
  2. Open browser and go to `http://192.168.4.1/update` (or click OTA in the header).
  3. Select `firmware.bin`.
  4. Click Flash Firmware Over-The-Air.
  5. The ESP32 writes the firmware and reboots automatically.

---

## 2. USB Flashing
- **File**: `merged_firmware.bin`
- **Offset**: `0x0000` (starts at 0x0)
- **Usage**:
  - Open Chrome on PC or Android.
  - Visit: https://espressif.github.io/esptool-js/
  - Set Baud Rate to `921600` or `460800`.
  - Select `merged_firmware.bin` at offset `0x0`.
  - Click Program.

---

## Technical Specifications
- **Device Name**: `ESP 32 C3`
- **Wi-Fi TX Power**: 11 dBm
- **BLE HID**: Keyboard + Mouse
- **Partition Scheme**: Dual OTA (`ota_nofs_4MB`, ~2 MB per app slot)
