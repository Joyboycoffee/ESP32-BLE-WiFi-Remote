# ESP 32 C3 Firmware Binaries

This folder contains compiled binaries for both Over-The-Air (wireless) updates and USB OTG (cable) flashing.

---

## 1. Web OTA Update (Wireless via Browser)
- **File**: `firmware.bin`
- **When to use**: Once your ESP32 is running firmware with OTA support, you can flash updates wirelessly from your phone or PC over Wi-Fi.
- **Instructions**:
  1. Connect your phone or PC to Wi-Fi **`ESP 32 C3`**.
  2. Open browser and go to `http://192.168.4.1/update` (or tap the **📡 OTA** button in the top header of the remote portal).
  3. Tap **Select firmware.bin** and choose `firmware.bin`.
  4. Tap **Flash Firmware Over-The-Air**.
  5. The ESP32 will write the firmware, verify it, and reboot automatically in ~5 seconds.

---

## 2. Mobile USB OTG Flashing (Direct Cable from Phone / Web Serial)
- **File**: `merged_firmware.bin`
- **Offset**: `0x0000` (starts at 0x0)
- **When to use**: Initial flash, unbricking, or cable update directly from an Android phone with a USB-C OTG cable without needing a PC.
- **Tools**:
  - **Option A (Web Browser - No App needed)**:
    - Open Chrome or Edge on PC or Android.
    - Go to: [Espressif Web Flasher](https://espressif.github.io/esptool-js/)
    - Baud Rate: `921600` or `460800`
    - Add file `merged_firmware.bin` at offset `0x0`.
    - Tap **Program**.
  - **Option B (Android App)**:
    - Install **ESP32 Flasher** or **Serial USB Terminal** with ESP plugin from Play Store.
    - Connect ESP32 via USB OTG cable.
    - Load `merged_firmware.bin` at address `0x0000` and flash.

---

## Technical Specifications
- **Device Name**: `ESP 32 C3`
- **Wi-Fi TX Power**: 11 dBm (stabilized to prevent 40MHz crystal clock pulling)
- **BLE HID**: Composite Keyboard + High-Report-Rate Mouse
- **Partition Scheme**: Dual OTA (`app0` / `app1` at 1.25 MB each)
