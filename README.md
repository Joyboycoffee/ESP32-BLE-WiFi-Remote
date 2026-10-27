# ESP 32 C3 (Wireless BLE Remote via Phone Web Portal)

An all-in-one wireless HID bridge powered by the **ESP32-C3**. The ESP32 creates its own Wi-Fi Hotspot named **`ESP 32 C3`** serving an interactive mobile touch portal featuring an **Air Remote (Touch Glide & Gyro Air Mouse)**, trackpad, keyboard, tactile console D-Pad, hotkeys, and media controls, while acting as a standard Bluetooth Low Energy (BLE) device named **`ESP 32 C3`** connected to your PC or Linux machine.

---

## 🚀 Features

- **🕹️ Dual-Mode Magic Remote**:
  - **Touch Glide Air Pad (Default)**: Inertial thumb glide with sub-pixel smoothing. Works 100% out-of-the-box over standard HTTP without flags or sensor permissions.
  - **Gyroscope Mode**: Physical air mouse using phone motion sensors (`rotationRate` / `devicemotion`) to steer the PC cursor like a TV magic remote.
  - Symmetrical Left & Right click zones, middle click, scroll buttons, and quick navigation shortcuts.
- **🎮 Console D-Pad with Auto-Repeat**:
  - Symmetrical directional cross with bold SVG arrow icons and center OK disc.
  - Smooth auto-repeat on hold (280ms initial delay, 75ms repeat rate) for seamless menu navigation.
- **🖱️ Precision Touchpad**:
  - Smooth relative pointer movements with adjustable sensitivity slider.
  - Tap for Left Click, two-finger tap for Right Click.
  - Dedicated vertical scroll strip for mouse wheel scrolling.
  - Left, Middle, and Right click buttons with hold-to-drag support.
- **⌨️ Virtual & Native Keyboard**:
  - Direct text input box: use your phone's native keyboard (Gboard, iOS, Samsung) with **voice typing**, dictation, and auto-correct.
  - Live keystroke mode or batch "Send" button.
  - Navigation cluster: Up, Down, Left, Right arrows, Home, End, PgUp, PgDn.
  - Special keys: Enter, Backspace, Tab, Esc, Space, Delete.
  - Sticky Modifiers: Ctrl, Alt, Shift, Win key (toggleable for combos).
- **⚡ Productivity Hotkeys**:
  - 1-tap `Ctrl+C`, `Ctrl+V`, `Ctrl+X`, `Ctrl+A`, `Ctrl+Z`.
  - Windows shortcuts: `Alt+Tab`, `Win+D` (Desktop), `Win+R` (Run), `Win+L` (Lock Screen).
  - Function keys: `F1` through `F12`.
- **🎵 Media Remote**:
  - Volume Up, Volume Down, Mute.
  - Play/Pause, Next Track, Previous Track.
- **📡 Over-The-Air (OTA) Updates**:
  - Wireless firmware upgrade portal at `http://192.168.4.1/update` or via the **`📡 OTA`** badge in the header.
  - Dual OTA partition scheme (`ota_nofs_4MB`) with live percentage progress bar and auto-reboot.
- **Zero App Installation**: Runs entirely inside mobile web browsers (Chrome, Safari, Firefox).
- **RF Tuning**: Wi-Fi TX power constrained to 11 dBm to prevent 40MHz crystal clock pulling on ESP32-C3 SuperMini boards.

---

## 📱 How to Use

### 1. Pair with your PC via Bluetooth
1. On your Windows PC, go to **Settings > Bluetooth & devices > Add device > Bluetooth**.
2. Select **`ESP 32 C3`** from the list.
3. Windows/Linux will pair with it immediately as a wireless input device.

### 2. Connect your Phone to the Wi-Fi Hotspot
1. On your phone, connect to Wi-Fi: **`ESP 32 C3`** (Open network, no password required).
2. Tap the **"Sign in to network"** notification, or open your browser and navigate to:
   ```
   http://192.168.4.1
   ```
3. *(Optional)* Tap browser menu `⋮` and select **Add to Home screen** to install it as an app.

---

## 📦 Precompiled Binaries

Precompiled binaries are available in the [`bin/`](bin/) folder:
- **`bin/firmware.bin`**: For Over-The-Air wireless updates via `http://192.168.4.1/update`.
- **`bin/merged_firmware.bin`**: Single combined binary at offset `0x0000` for mobile USB OTG flashing (via [esptool-js](https://espressif.github.io/esptool-js/) or ESP Flasher app).

---

## 🔧 Building & Flashing

```powershell
# Flash directly via PlatformIO
pio run -t upload
```
