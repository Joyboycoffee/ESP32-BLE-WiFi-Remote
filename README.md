# ESP 32 C3 (Wireless BLE Remote via Phone Web Portal)

An all-in-one wireless HID bridge powered by the ESP32-C3. The ESP32 creates a Wi-Fi Hotspot named `ESP 32 C3` serving a mobile touch portal featuring an Air Remote (Touch Glide & Gyro Air Mouse), trackpad, keyboard, console D-Pad, hotkeys, and media controls, while acting as a standard Bluetooth Low Energy (BLE) HID device named `ESP 32 C3` connected to a PC or Linux machine.

---

## Features

- **Dual-Mode Air Remote**:
  - **Touch Glide Air Pad**: Inertial thumb glide with sub-pixel smoothing. Works over standard HTTP without sensor permissions.
  - **Gyroscope Mode**: Physical air mouse using phone motion sensors (`rotationRate` / `devicemotion`) to steer the PC cursor.
  - Symmetrical left and right click zones, middle click, scroll buttons, and quick navigation shortcuts.
- **Console D-Pad**:
  - Directional cross with center OK disc.
  - Auto-repeat on hold (280ms initial delay, 75ms repeat rate).
- **Touchpad**:
  - Relative pointer movements with adjustable sensitivity slider.
  - Tap for left click, two-finger tap for right click.
  - Vertical scroll strip for mouse wheel scrolling.
  - Left, middle, and right click buttons with drag support.
- **Keyboard**:
  - Text input box supporting phone keyboard, voice typing, and auto-correct.
  - Navigation keys: Up, Down, Left, Right, Home, End, Page Up, Page Down.
  - Control keys: Enter, Backspace, Tab, Escape, Space, Delete.
  - Sticky modifiers: Ctrl, Alt, Shift, GUI.
- **Productivity Hotkeys**:
  - Shortcuts: Ctrl+C, Ctrl+V, Ctrl+X, Ctrl+A, Ctrl+Z.
  - System shortcuts: Alt+Tab, Win+D, Win+R, Win+L.
  - Function keys: F1 through F12.
- **Media Remote**:
  - Volume Up, Volume Down, Mute.
  - Play/Pause, Next Track, Previous Track.
- **Over-The-Air (OTA) Updates**:
  - Firmware upgrade portal at `http://192.168.4.1/update` and linked from the web interface header.
  - Dual OTA partition scheme (`ota_nofs_4MB`) with upload progress indicator and auto-reboot.
- **RF Tuning**: Wi-Fi TX power set to 11 dBm to prevent crystal clock pulling.

---

## How to Use

### 1. Pair with PC via Bluetooth
1. Go to Bluetooth settings on the host computer.
2. Select and pair with `ESP 32 C3`.

### 2. Connect Phone to the Wi-Fi Hotspot
1. Connect phone to Wi-Fi network `ESP 32 C3` (open network, no password).
2. Open a browser and navigate to:
   ```
   http://192.168.4.1
   ```

---

## Precompiled Binaries

Precompiled binaries are located in the [`bin/`](bin/) directory:
- `bin/firmware.bin`: For Over-The-Air updates via `http://192.168.4.1/update`.
- `bin/merged_firmware.bin`: Complete image at offset `0x0000` for USB flashing.

---

## Building and Flashing

```powershell
pio run -t upload
```
