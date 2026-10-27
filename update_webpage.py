import gzip
import os

html_content = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
<title>ESP 32 C3</title>
<style>
:root {
  --bg-color: #080c14;
  --surface: #111827;
  --surface-alt: #1f293d;
  --border: #2e3c54;
  --border-focus: #38bdf8;
  --text-main: #f8fafc;
  --text-dim: #94a3b8;
  --accent: #38bdf8;
  --accent-glow: rgba(56, 189, 248, 0.4);
  --magic: #a855f7;
  --magic-glow: rgba(168, 85, 247, 0.45);
  --success: #22c55e;
  --danger: #ef4444;
  --radius-lg: 16px;
  --radius-md: 10px;
  --radius-sm: 6px;
}
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
  user-select: none;
  -webkit-user-select: none;
  -webkit-tap-highlight-color: transparent;
}
body {
  background-color: var(--bg-color);
  color: var(--text-main);
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
  height: 100vh;
  height: 100dvh;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}
header {
  background: var(--surface);
  border-bottom: 1px solid var(--border);
  padding: 10px 16px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-shrink: 0;
}
.header-title {
  display: flex;
  align-items: center;
  gap: 8px;
  font-weight: 700;
  font-size: 17px;
  letter-spacing: 0.5px;
}
.status-badge {
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 11px;
  font-weight: 600;
  padding: 4px 10px;
  border-radius: 999px;
  background: var(--surface-alt);
  border: 1px solid var(--border);
}
.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  background: var(--danger);
  transition: all 0.3s;
}
.status-dot.active {
  background: var(--success);
  box-shadow: 0 0 8px var(--success);
}
.tab-bar {
  display: flex;
  background: var(--surface);
  border-bottom: 1px solid var(--border);
  flex-shrink: 0;
  overflow-x: auto;
  scrollbar-width: none;
}
.tab-bar::-webkit-scrollbar { display: none; }
.tab-btn {
  flex: 1;
  min-width: 70px;
  padding: 10px 4px;
  background: transparent;
  border: none;
  border-bottom: 2px solid transparent;
  color: var(--text-dim);
  font-size: 12px;
  font-weight: 600;
  cursor: pointer;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 2px;
  white-space: nowrap;
  transition: all 0.15s ease;
}
.tab-btn.active {
  color: var(--accent);
  border-bottom-color: var(--accent);
}
.tab-btn.magic-tab.active {
  color: var(--magic);
  border-bottom-color: var(--magic);
}
.content-area {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow-y: auto;
  position: relative;
}
.tab-pane {
  display: none;
  flex: 1;
  flex-direction: column;
  padding: 12px;
  gap: 12px;
}
.tab-pane.active {
  display: flex;
}
.btn {
  background: var(--surface);
  border: 1px solid var(--border);
  color: var(--text-main);
  border-radius: var(--radius-md);
  padding: 10px;
  font-weight: 600;
  font-size: 13px;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  transition: all 0.1s;
}
.btn:active, .btn.active {
  background: var(--surface-alt);
  border-color: var(--accent);
  transform: scale(0.97);
}

/* MAGIC REMOTE TAB STYLES */
.magic-container {
  display: flex;
  flex-direction: column;
  gap: 10px;
  max-width: 480px;
  margin: 0 auto;
  width: 100%;
}
.mode-pill-selector {
  display: flex;
  background: var(--surface);
  border: 1px solid var(--border);
  border-radius: 999px;
  padding: 3px;
  gap: 4px;
}
.mode-pill {
  flex: 1;
  padding: 7px 10px;
  border-radius: 999px;
  background: transparent;
  border: none;
  color: var(--text-dim);
  font-size: 12px;
  font-weight: 700;
  cursor: pointer;
  text-align: center;
  transition: all 0.2s;
}
.mode-pill.active {
  background: var(--magic);
  color: #fff;
  box-shadow: 0 0 12px var(--magic-glow);
}
.magic-surface {
  position: relative;
  height: 210px;
  background: radial-gradient(circle at center, #1e1538 0%, #111424 100%);
  border: 2px dashed rgba(168, 85, 247, 0.4);
  border-radius: var(--radius-lg);
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  touch-action: none;
  cursor: grab;
  overflow: hidden;
  box-shadow: inset 0 0 25px rgba(0, 0, 0, 0.6);
}
.magic-surface.active {
  border-color: var(--magic);
  border-style: solid;
  box-shadow: 0 0 20px var(--magic-glow), inset 0 0 30px rgba(168, 85, 247, 0.2);
}
.magic-crosshair {
  position: absolute;
  width: 140px;
  height: 140px;
  border-radius: 50%;
  border: 1px solid rgba(168, 85, 247, 0.3);
  pointer-events: none;
  display: flex;
  align-items: center;
  justify-content: center;
}
.magic-crosshair::before, .magic-crosshair::after {
  content: '';
  position: absolute;
  background: rgba(168, 85, 247, 0.25);
}
.magic-crosshair::before { width: 100%; height: 1px; }
.magic-crosshair::after { width: 1px; height: 100%; }
.magic-reticle-dot {
  width: 18px;
  height: 18px;
  border-radius: 50%;
  background: var(--magic);
  box-shadow: 0 0 14px var(--magic);
  transition: transform 0.05s ease-out;
  pointer-events: none;
}
.magic-surface-label {
  position: absolute;
  bottom: 12px;
  font-size: 11px;
  font-weight: 700;
  letter-spacing: 0.5px;
  color: #e9d5ff;
  text-transform: uppercase;
  pointer-events: none;
  background: rgba(17, 20, 36, 0.7);
  padding: 3px 10px;
  border-radius: 12px;
  border: 1px solid rgba(168, 85, 247, 0.3);
}
.magic-clicks-row {
  display: grid;
  grid-template-columns: 1fr 68px 1fr;
  gap: 8px;
}
.magic-btn-left, .magic-btn-right {
  height: 72px;
  font-size: 14px;
  font-weight: 800;
  letter-spacing: 0.5px;
}
.magic-btn-left {
  background: linear-gradient(135deg, #1e1b4b, #2e1065);
  border-color: var(--magic);
  color: #f3e8ff;
}
.magic-btn-left:active {
  background: var(--magic);
  color: #fff;
  box-shadow: 0 0 20px var(--magic-glow);
}
.magic-btn-right {
  background: var(--surface);
  border-color: var(--border);
  color: var(--text-main);
}
.magic-btn-right:active {
  background: var(--surface-alt);
  border-color: var(--accent);
}
.magic-wheel-col {
  display: flex;
  flex-direction: column;
  gap: 4px;
}
.wheel-btn {
  flex: 1;
  padding: 0;
  font-size: 11px;
  font-weight: 700;
  border-radius: var(--radius-sm);
  background: var(--surface-alt);
}
.magic-nav-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 6px;
}
.magic-nav-btn {
  height: 42px;
  font-size: 12px;
  font-weight: 600;
  padding: 4px;
}
.slider-group {
  display: flex;
  align-items: center;
  justify-content: space-between;
  background: var(--surface);
  border: 1px solid var(--border);
  border-radius: var(--radius-md);
  padding: 10px 14px;
  font-size: 12px;
  font-weight: 600;
  gap: 12px;
}
.slider-group input[type="range"] {
  flex: 1;
  height: 6px;
  accent-color: var(--magic);
}
.notice-box {
  background: rgba(168, 85, 247, 0.1);
  border: 1px solid var(--magic);
  border-radius: var(--radius-md);
  padding: 10px 12px;
  font-size: 12px;
  line-height: 1.4;
  color: #e9d5ff;
}

/* KEYBOARD & DPAD STYLES */
.input-card {
  background: var(--surface);
  border: 1px solid var(--border);
  border-radius: var(--radius-md);
  padding: 10px;
  display: flex;
  flex-direction: column;
  gap: 8px;
}
.text-input-row {
  display: flex;
  gap: 6px;
}
#text-input {
  flex: 1;
  background: var(--surface-alt);
  border: 1px solid var(--border);
  border-radius: var(--radius-sm);
  padding: 10px 12px;
  color: var(--text-main);
  font-size: 14px;
  outline: none;
}
#text-input:focus {
  border-color: var(--accent);
}
.keys-grid {
  display: grid;
  grid-template-columns: repeat(4, 1fr);
  gap: 6px;
}
.keys-grid.dense {
  grid-template-columns: repeat(6, 1fr);
}
.key-btn {
  min-height: 42px;
  font-size: 12px;
}

/* CONSOLE-GRADE ERGONOMIC D-PAD */
.dpad-container {
  position: relative;
  width: 220px;
  height: 220px;
  margin: 14px auto 6px auto;
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  grid-template-rows: 1fr 1fr 1fr;
  gap: 6px;
  align-items: center;
  justify-items: center;
}
.dpad-btn {
  display: flex;
  align-items: center;
  justify-content: center;
  background: var(--surface);
  border: 1px solid var(--border);
  color: var(--text-main);
  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.35);
  cursor: pointer;
  user-select: none;
  -webkit-user-select: none;
  touch-action: none;
  transition: all 0.08s ease;
}
.dpad-btn:active, .dpad-btn.active {
  background: var(--accent);
  color: #0b0f19;
  border-color: var(--accent);
  transform: scale(0.92);
  box-shadow: 0 0 16px var(--accent-glow);
}
.dpad-btn svg {
  width: 26px;
  height: 26px;
  fill: currentColor;
  pointer-events: none;
}
.dpad-up {
  grid-column: 2;
  grid-row: 1;
  width: 68px;
  height: 62px;
  border-radius: 16px 16px 6px 6px;
}
.dpad-left {
  grid-column: 1;
  grid-row: 2;
  width: 62px;
  height: 68px;
  border-radius: 16px 6px 6px 16px;
}
.dpad-center {
  grid-column: 2;
  grid-row: 2;
  width: 64px;
  height: 64px;
  border-radius: 50%;
  background: var(--surface-alt);
  border: 2px solid var(--accent);
  color: var(--accent);
  font-weight: 800;
  font-size: 13px;
  box-shadow: 0 0 12px rgba(56, 189, 248, 0.25);
}
.dpad-center:active {
  background: var(--accent);
  color: #0b0f19;
  transform: scale(0.9);
}
.dpad-right {
  grid-column: 3;
  grid-row: 2;
  width: 62px;
  height: 68px;
  border-radius: 6px 16px 16px 6px;
}
.dpad-down {
  grid-column: 2;
  grid-row: 3;
  width: 68px;
  height: 62px;
  border-radius: 6px 6px 16px 16px;
}
.dpad-nav-row {
  display: flex;
  justify-content: center;
  gap: 8px;
  max-width: 320px;
  margin: 0 auto;
  width: 100%;
}
.dpad-nav-btn {
  flex: 1;
  height: 38px;
  font-size: 11px;
  font-weight: 600;
}

/* TOUCHPAD TAB */
#trackpad {
  flex: 1;
  min-height: 240px;
  background: radial-gradient(circle at center, #1a2336 0%, #0f1624 100%);
  border: 1px solid var(--border);
  border-radius: var(--radius-lg);
  touch-action: none;
  cursor: crosshair;
  position: relative;
  display: flex;
  align-items: center;
  justify-content: center;
}
#trackpad::after {
  content: "Precision Trackpad \\A (Tap: Left Click | 2-Finger: Right Click)";
  white-space: pre-wrap;
  text-align: center;
  font-size: 12px;
  font-weight: 500;
  color: var(--text-dim);
  opacity: 0.6;
  pointer-events: none;
}
.mouse-buttons-row {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  gap: 8px;
}
.mouse-btn-left, .mouse-btn-right {
  height: 52px;
}
.mouse-btn-mid {
  height: 52px;
  font-size: 11px;
}

/* MEDIA TAB */
.media-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 10px;
  margin-top: 10px;
}
.media-btn {
  height: 68px;
  font-size: 22px;
  flex-direction: column;
  gap: 4px;
}
.media-btn span {
  font-size: 10px;
  font-weight: 600;
  color: var(--text-dim);
}
</style>
</head>
<body>

<header>
  <div class="header-title">
    <span>&#128250;</span>
    <span>ESP 32 C3</span>
  </div>
  <div style="display: flex; align-items: center; gap: 8px;">
    <a href="/update" style="display: flex; align-items: center; gap: 4px; font-size: 11px; font-weight: 600; padding: 4px 8px; border-radius: 999px; background: rgba(56, 189, 248, 0.15); border: 1px solid var(--accent); color: var(--accent); text-decoration: none;">
      <span>&#128225;</span><span>OTA</span>
    </a>
    <div class="status-badge">
      <div class="status-dot" id="ble-dot"></div>
      <span id="ble-text">Connecting...</span>
    </div>
  </div>
</header>

<div class="tab-bar">
  <button class="tab-btn magic-tab active" onclick="showTab('magic')">
    <span>&#129668;</span>
    <span>Magic Remote</span>
  </button>
  <button class="tab-btn" onclick="showTab('mouse')">
    <span>&#128433;</span>
    <span>Trackpad</span>
  </button>
  <button class="tab-btn" onclick="showTab('keyboard')">
    <span>&#9000;</span>
    <span>Keyboard</span>
  </button>
  <button class="tab-btn" onclick="showTab('shortcuts')">
    <span>&#9889;</span>
    <span>Shortcuts</span>
  </button>
  <button class="tab-btn" onclick="showTab('media')">
    <span>&#127911;</span>
    <span>Media</span>
  </button>
</div>

<div class="content-area">

  <!-- 1. MAGIC REMOTE TAB -->
  <div class="tab-pane active" id="pane-magic">
    <div class="magic-container">
      
      <!-- MODE SWITCHER -->
      <div class="mode-pill-selector">
        <button class="mode-pill active" id="pill-touch" onclick="setMagicMode('touch')">🕹️ Touch Glide Air Pad</button>
        <button class="mode-pill" id="pill-gyro" onclick="setMagicMode('gyro')">📱 Gyroscope Mode</button>
      </div>

      <!-- INSECURE ORIGIN NOTICE (Only shown in Gyro mode if browser blocks sensors) -->
      <div class="notice-box" id="gyro-notice" style="display: none;">
        <div style="font-weight: 700; margin-bottom: 4px;">🔒 Chrome Sensor Notice</div>
        <div>Chrome blocks phone gyro on HTTP. Enable it in 10 sec:</div>
        <div style="font-size: 11px; margin: 4px 0; color: #fdf4ff;">
          1. Go to: <code>chrome://flags/#unsafely-treat-insecure-origin-as-secure</code><br>
          2. Add <code>http://192.168.4.1</code> & set to <b>Enabled</b><br>
          3. Relaunch Chrome.
        </div>
        <div style="display: flex; gap: 6px; margin-top: 6px;">
          <button class="btn" style="flex: 1; padding: 5px; font-size: 11px; background: var(--magic); color: #fff;" onclick="copyOrigin()">Copy Origin URL</button>
          <button class="btn" style="flex: 1; padding: 5px; font-size: 11px;" onclick="setMagicMode('touch')">Use Touch Pad</button>
        </div>
      </div>

      <!-- CENTRAL MAGIC SURFACE (TOUCH GLIDE / GYRO RETICLE) -->
      <div class="magic-surface" id="magic-surface">
        <div class="magic-crosshair">
          <div class="magic-reticle-dot" id="magic-dot"></div>
        </div>
        <div class="magic-surface-label" id="magic-label">Glide Thumb to Move Mouse &bull; Tap: Left Click</div>
      </div>

      <!-- CLICK & SCROLL BUTTONS -->
      <div class="magic-clicks-row">
        <button class="btn magic-btn-left" id="magic-left-click">LEFT CLICK</button>
        <div class="magic-wheel-col">
          <button class="btn wheel-btn" onclick="sendScroll(1)">&#9650; Scroll</button>
          <button class="btn wheel-btn" id="magic-mid-click">Middle</button>
          <button class="btn wheel-btn" onclick="sendScroll(-1)">&#9660; Scroll</button>
        </div>
        <button class="btn magic-btn-right" id="magic-right-click">RIGHT CLICK</button>
      </div>

      <!-- QUICK REMOTE NAV ROW -->
      <div class="magic-nav-grid">
        <button class="btn magic-nav-btn" onclick="sendKey('ESC')">&#8592; Back (Esc)</button>
        <button class="btn magic-nav-btn" onclick="sendKey('ENTER')">OK (Enter)</button>
        <button class="btn magic-nav-btn" onclick="sendCombo('WIN_D')">&#8862; Desktop</button>
        <button class="btn magic-nav-btn" onclick="sendMedia('VOL_UP')">&#128266; Vol +</button>
        <button class="btn magic-nav-btn" onclick="sendMedia('VOL_DOWN')">&#128265; Vol -</button>
        <button class="btn magic-nav-btn" onclick="sendMedia('MUTE')">&#128263; Mute</button>
        <button class="btn magic-nav-btn" onclick="sendMedia('PLAY_PAUSE')">&#9199; Play/Pause</button>
        <button class="btn magic-nav-btn" onclick="sendCombo('ALT_TAB')">&#8644; Alt+Tab</button>
      </div>

      <!-- SPEED SLIDER -->
      <div class="slider-group">
        <span>Remote Speed</span>
        <input type="range" id="magic-sens-slider" min="0.8" max="4.0" step="0.2" value="2.0">
        <span id="magic-sens-val">2.0x</span>
      </div>

    </div>
  </div>

  <!-- 2. TOUCHPAD TAB -->
  <div class="tab-pane" id="pane-mouse">
    <div id="trackpad"></div>
    <div class="mouse-buttons-row">
      <button class="btn mouse-btn-left" id="btn-left-click">LEFT CLICK</button>
      <button class="btn mouse-btn-mid" id="btn-mid-click">MIDDLE</button>
      <button class="btn mouse-btn-right" id="btn-right-click">RIGHT CLICK</button>
    </div>
    <div class="slider-group">
      <span>Trackpad Sensitivity</span>
      <input type="range" id="sens-slider" min="0.8" max="3.0" step="0.2" value="1.6">
      <span id="sens-val">1.6x</span>
    </div>
  </div>

  <!-- 3. KEYBOARD TAB -->
  <div class="tab-pane" id="pane-keyboard">
    <div class="input-card">
      <div class="text-input-row">
        <input type="text" id="text-input" placeholder="Type text or use phone keyboard...">
        <button class="btn" style="padding: 0 16px; background: var(--accent); color: #000;" onclick="sendInputText()">Send</button>
      </div>
      <div style="display: flex; gap: 8px; justify-content: flex-end;">
        <label style="font-size: 11px; display: flex; align-items: center; gap: 4px; color: var(--text-dim);">
          <input type="checkbox" id="live-typing" checked> Live keystroke mode
        </label>
      </div>
    </div>

    <div class="keys-grid">
      <button class="btn key-btn" onclick="sendKey('ENTER')">Enter &#x23CE;</button>
      <button class="btn key-btn" onclick="sendKey('BACKSPACE')">Backspace &#x232B;</button>
      <button class="btn key-btn" onclick="sendKey('TAB')">Tab &#x21E5;</button>
      <button class="btn key-btn" onclick="sendKey('ESC')">Esc</button>
      <button class="btn key-btn" onclick="sendKey('SPACE')">Space &#x2423;</button>
      <button class="btn key-btn" onclick="sendKey('DELETE')">Delete &#x2326;</button>
      <button class="btn key-btn" onclick="toggleMod('WIN')" id="mod-WIN">Win &#x229E;</button>
      <button class="btn key-btn" onclick="toggleMod('CTRL')" id="mod-CTRL">Ctrl</button>
      <button class="btn key-btn" onclick="toggleMod('ALT')" id="mod-ALT">Alt</button>
      <button class="btn key-btn" onclick="toggleMod('SHIFT')" id="mod-SHIFT">Shift</button>
    </div>

    <!-- CONSOLE-GRADE D-PAD WITH AUTO-REPEAT ON HOLD -->
    <div style="margin-top: 6px; text-align: center; font-size: 12px; font-weight: 700; color: var(--text-dim); letter-spacing: 0.5px;">
      DIRECTIONAL CONTROLLER
    </div>

    <div class="dpad-container">
      <div class="dpad-btn dpad-up" id="dpad-btn-up">
        <svg viewBox="0 0 24 24"><path d="M12 4l-8 8h5v8h6v-8h5z"/></svg>
      </div>
      <div class="dpad-btn dpad-left" id="dpad-btn-left">
        <svg viewBox="0 0 24 24"><path d="M4 12l8-8v5h8v6h-8v5z"/></svg>
      </div>
      <div class="dpad-btn dpad-center" onclick="sendKey('ENTER'); vibrate(20);">OK</div>
      <div class="dpad-btn dpad-right" id="dpad-btn-right">
        <svg viewBox="0 0 24 24"><path d="M20 12l-8 8v-5H4v-6h8V4z"/></svg>
      </div>
      <div class="dpad-btn dpad-down" id="dpad-btn-down">
        <svg viewBox="0 0 24 24"><path d="M12 20l8-8h-5V4h-6v8H4z"/></svg>
      </div>
    </div>

    <div class="dpad-nav-row">
      <button class="btn dpad-nav-btn" onclick="sendKey('PAGEUP')">PgUp &#x21DE;</button>
      <button class="btn dpad-nav-btn" onclick="sendKey('PAGEDOWN')">PgDn &#x21DF;</button>
      <button class="btn dpad-nav-btn" onclick="sendKey('HOME')">Home</button>
      <button class="btn dpad-nav-btn" onclick="sendKey('END')">End</button>
    </div>

  </div>

  <!-- 4. SHORTCUTS TAB -->
  <div class="tab-pane" id="pane-shortcuts">
    <div style="font-size: 12px; color: var(--text-dim); margin-bottom: 4px;">Quick Actions & Hotkeys</div>
    <div class="keys-grid">
      <button class="btn key-btn" onclick="sendCombo('COPY')">Ctrl + C (Copy)</button>
      <button class="btn key-btn" onclick="sendCombo('PASTE')">Ctrl + V (Paste)</button>
      <button class="btn key-btn" onclick="sendCombo('CUT')">Ctrl + X (Cut)</button>
      <button class="btn key-btn" onclick="sendCombo('SELECT_ALL')">Ctrl + A (All)</button>
      <button class="btn key-btn" onclick="sendCombo('UNDO')">Ctrl + Z (Undo)</button>
      <button class="btn key-btn" onclick="sendCombo('ALT_TAB')">Alt + Tab</button>
      <button class="btn key-btn" onclick="sendCombo('WIN_D')">Win + D (Desktop)</button>
      <button class="btn key-btn" onclick="sendCombo('WIN_R')">Win + R (Run)</button>
      <button class="btn key-btn" onclick="sendCombo('LOCK')">Win + L (Lock)</button>
      <button class="btn key-btn" onclick="sendKey('F11')">F11 (Fullscr)</button>
    </div>

    <div style="font-size: 12px; color: var(--text-dim); margin-top: 12px; margin-bottom: 4px;">Function Keys</div>
    <div class="keys-grid dense">
      <button class="btn key-btn" onclick="sendKey('F1')">F1</button>
      <button class="btn key-btn" onclick="sendKey('F2')">F2</button>
      <button class="btn key-btn" onclick="sendKey('F3')">F3</button>
      <button class="btn key-btn" onclick="sendKey('F4')">F4</button>
      <button class="btn key-btn" onclick="sendKey('F5')">F5</button>
      <button class="btn key-btn" onclick="sendKey('F6')">F6</button>
      <button class="btn key-btn" onclick="sendKey('F7')">F7</button>
      <button class="btn key-btn" onclick="sendKey('F8')">F8</button>
      <button class="btn key-btn" onclick="sendKey('F9')">F9</button>
      <button class="btn key-btn" onclick="sendKey('F10')">F10</button>
      <button class="btn key-btn" onclick="sendKey('F11')">F11</button>
      <button class="btn key-btn" onclick="sendKey('F12')">F12</button>
    </div>
  </div>

  <!-- 5. MEDIA TAB -->
  <div class="tab-pane" id="pane-media">
    <div style="font-size: 12px; color: var(--text-dim);">Media & Volume Playback</div>
    <div class="media-grid">
      <button class="btn media-btn" onclick="sendMedia('VOL_UP')">&#128266; <span>Vol +</span></button>
      <button class="btn media-btn" onclick="sendMedia('PLAY_PAUSE')">&#9199; <span>Play/Pause</span></button>
      <button class="btn media-btn" onclick="sendMedia('VOL_DOWN')">&#128265; <span>Vol -</span></button>
      <button class="btn media-btn" onclick="sendMedia('PREV')">&#9198; <span>Prev</span></button>
      <button class="btn media-btn" onclick="sendMedia('MUTE')">&#128263; <span>Mute</span></button>
      <button class="btn media-btn" onclick="sendMedia('NEXT')">&#9197; <span>Next</span></button>
    </div>
  </div>

</div>

<script>
// WebSocket Connection
let ws;
const bleDot = document.getElementById('ble-dot');
const bleText = document.getElementById('ble-text');

function initWebSocket() {
  const wsHost = window.location.hostname || "192.168.4.1";
  ws = new WebSocket("ws://" + wsHost + ":81");

  ws.onopen = () => {
    console.log("WebSocket connected");
  };

  ws.onclose = () => {
    bleDot.classList.remove('active');
    bleText.innerText = "Reconnecting...";
    setTimeout(initWebSocket, 1500);
  };

  ws.onmessage = (event) => {
    const data = event.data;
    if (data.startsWith("STATUS:BLE=")) {
      const isBleConn = data.substring(11, 12) === "1";
      if (isBleConn) {
        bleDot.classList.add('active');
        bleText.innerText = "BLE: Connected to PC";
      } else {
        bleDot.classList.remove('active');
        bleText.innerText = "BLE: Pair with PC";
      }
    }
  };
}
initWebSocket();

function send(msg) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(msg);
  }
}

function vibrate(ms) {
  if (navigator.vibrate) navigator.vibrate(ms);
}

// -------------------------------------------------------------
// BUTTER-SMOOTH 60 FPS MOTION ACCUMULATOR (NO STUTTER)
// -------------------------------------------------------------
let pendingDx = 0;
let pendingDy = 0;
let rafActive = false;

function queueMouseMove(rawDx, rawDy) {
  // Non-linear acceleration:
  // Micro-movements (<3px) are 1:1 for pixel precision
  // Larger thumb sweeps accelerate smoothly to traverse screens
  const dist = Math.hypot(rawDx, rawDy);
  let accel = 1.0;
  if (dist > 2.5) {
    accel = 1.0 + Math.min((dist - 2.5) * 0.08, 1.8);
  }

  pendingDx += rawDx * accel;
  pendingDy += rawDy * accel;

  if (!rafActive) {
    rafActive = true;
    requestAnimationFrame(flushMouseMove);
  }
}

function flushMouseMove() {
  rafActive = false;
  // Send integer part, retain fractional sub-pixel remainder for slow creeping movements
  const sendDx = Math.trunc(pendingDx);
  const sendDy = Math.trunc(pendingDy);

  if (sendDx !== 0 || sendDy !== 0) {
    send("M:" + sendDx + ":" + sendDy);
    pendingDx -= sendDx;
    pendingDy -= sendDy;
  }
}

// Tab Switching
function showTab(name) {
  document.querySelectorAll('.tab-pane').forEach(el => el.classList.remove('active'));
  document.querySelectorAll('.tab-btn').forEach(el => el.classList.remove('active'));
  document.getElementById('pane-' + name).classList.add('active');
  const btns = document.querySelectorAll('.tab-btn');
  const order = ['magic', 'mouse', 'keyboard', 'shortcuts', 'media'];
  const idx = order.indexOf(name);
  if (idx >= 0 && btns[idx]) btns[idx].classList.add('active');
  vibrate(15);
}

// -------------------------------------------------------------
// MAGIC REMOTE (DUAL: TOUCH GLIDE + GYROSCOPE)
// -------------------------------------------------------------
let magicMode = 'touch'; // 'touch' or 'gyro'
let magicSens = 2.0;
const magicSurface = document.getElementById('magic-surface');
const magicDot = document.getElementById('magic-dot');
const magicLabel = document.getElementById('magic-label');
const gyroNotice = document.getElementById('gyro-notice');
const magicSensSlider = document.getElementById('magic-sens-slider');
const magicSensVal = document.getElementById('magic-sens-val');

magicSensSlider.addEventListener('input', (e) => {
  magicSens = parseFloat(e.target.value);
  magicSensVal.innerText = magicSens.toFixed(1) + 'x';
});

function setMagicMode(mode) {
  magicMode = mode;
  document.getElementById('pill-touch').classList.toggle('active', mode === 'touch');
  document.getElementById('pill-gyro').classList.toggle('active', mode === 'gyro');
  vibrate(20);

  if (mode === 'touch') {
    gyroNotice.style.display = 'none';
    magicSurface.classList.remove('active');
    magicLabel.innerText = "Glide Thumb to Move Mouse &bull; Tap: Left Click";
    magicDot.style.transform = 'translate(0px, 0px)';
  } else {
    magicLabel.innerText = "Wave Phone in Air to Move Pointer";
    checkGyroAvailability();
  }
}

function copyOrigin() {
  const origin = window.location.origin || "http://192.168.4.1";
  navigator.clipboard.writeText(origin).then(() => {
    alert("Copied: " + origin + "\\nPaste into Chrome flags.");
  }).catch(() => {
    prompt("Copy this URL:", origin);
  });
}

// Touch Glide on Magic Surface
let mTouchActive = false;
let mLastX = 0, mLastY = 0;
let mTouchStartTime = 0;
let mTotalDist = 0;

magicSurface.addEventListener('pointerdown', (e) => {
  if (magicMode !== 'touch') return;
  magicSurface.setPointerCapture(e.pointerId);
  mTouchActive = true;
  mLastX = e.clientX;
  mLastY = e.clientY;
  mTouchStartTime = Date.now();
  mTotalDist = 0;
  magicSurface.classList.add('active');
});

magicSurface.addEventListener('pointermove', (e) => {
  if (!mTouchActive || magicMode !== 'touch') return;
  const rawDx = (e.clientX - mLastX) * magicSens;
  const rawDy = (e.clientY - mLastY) * magicSens;
  mLastX = e.clientX;
  mLastY = e.clientY;

  mTotalDist += Math.abs(rawDx) + Math.abs(rawDy);

  // Update reticle visual dot position (bounded)
  const rect = magicSurface.getBoundingClientRect();
  const relX = Math.max(-50, Math.min(50, (e.clientX - rect.left - rect.width / 2) * 0.4));
  const relY = Math.max(-50, Math.min(50, (e.clientY - rect.top - rect.height / 2) * 0.4));
  magicDot.style.transform = `translate(${relX}px, ${relY}px)`;

  queueMouseMove(rawDx, rawDy);
});

function handleMagicPointerEnd() {
  if (!mTouchActive || magicMode !== 'touch') return;
  mTouchActive = false;
  magicSurface.classList.remove('active');
  magicDot.style.transform = 'translate(0px, 0px)';
  const duration = Date.now() - mTouchStartTime;
  if (duration < 250 && mTotalDist < 10) {
    send("C:L");
    vibrate(25);
  }
}
magicSurface.addEventListener('pointerup', handleMagicPointerEnd);
magicSurface.addEventListener('pointercancel', handleMagicPointerEnd);

// Gyroscope tracking logic
let gyroListenerActive = false;
let gyroReceivedData = false;

function checkGyroAvailability() {
  if (!window.isSecureContext) {
    gyroNotice.style.display = 'block';
  }
  startGyroTracking();
}

function startGyroTracking() {
  if (gyroListenerActive) return;
  if (typeof DeviceMotionEvent !== 'undefined' && typeof DeviceMotionEvent.requestPermission === 'function') {
    DeviceMotionEvent.requestPermission().then(st => {
      if (st === 'granted') attachGyroEvents();
    }).catch(console.error);
  } else {
    attachGyroEvents();
  }
}

function attachGyroEvents() {
  window.addEventListener('devicemotion', onDeviceMotion, { passive: true });
  window.addEventListener('deviceorientation', onOrientationFallback, { passive: true });
  gyroListenerActive = true;
  magicSurface.classList.add('active');
}

function onDeviceMotion(e) {
  if (magicMode !== 'gyro') return;
  const rr = e.rotationRate;
  if (!rr || (rr.alpha === null && rr.beta === null && rr.gamma === null)) return;
  gyroReceivedData = true;
  gyroNotice.style.display = 'none';

  let pitch = rr.beta || 0;
  let yaw = rr.gamma || 0;

  if (Math.abs(pitch) < 0.35) pitch = 0;
  if (Math.abs(yaw) < 0.35) yaw = 0;
  if (pitch === 0 && yaw === 0) return;

  let dx = -yaw * magicSens * 1.6;
  let dy = -pitch * magicSens * 1.6;

  // Reticle feedback
  const rX = Math.max(-45, Math.min(45, -yaw * 4));
  const rY = Math.max(-45, Math.min(45, -pitch * 4));
  magicDot.style.transform = `translate(${rX}px, ${rY}px)`;

  queueMouseMove(dx, dy);
}

let lastOBeta = null, lastOGamma = null;
function onOrientationFallback(e) {
  if (magicMode !== 'gyro' || gyroReceivedData) return;
  if (e.beta === null) return;
  gyroReceivedData = true;
  gyroNotice.style.display = 'none';

  if (lastOBeta === null) {
    lastOBeta = e.beta;
    lastOGamma = e.gamma;
    return;
  }
  let dBeta = e.beta - lastOBeta;
  let dGamma = e.gamma - lastOGamma;
  lastOBeta = e.beta;
  lastOGamma = e.gamma;

  if (Math.abs(dBeta) > 30 || Math.abs(dGamma) > 30) return;
  if (Math.abs(dBeta) < 0.25) dBeta = 0;
  if (Math.abs(dGamma) < 0.25) dGamma = 0;

  let dx = dGamma * magicSens * 2.5;
  let dy = -dBeta * magicSens * 2.5;

  const rX = Math.max(-45, Math.min(45, dGamma * 8));
  const rY = Math.max(-45, Math.min(45, -dBeta * 8));
  magicDot.style.transform = `translate(${rX}px, ${rY}px)`;

  queueMouseMove(dx, dy);
}

// Button Events helper
function setupButtonEvents(id, btnCode) {
  const el = document.getElementById(id);
  if (!el) return;
  el.addEventListener('pointerdown', (e) => {
    e.preventDefault();
    send("MD:" + btnCode);
    vibrate(25);
    el.classList.add('active');
  });
  const up = (e) => {
    e.preventDefault();
    send("MU:" + btnCode);
    el.classList.remove('active');
  };
  el.addEventListener('pointerup', up);
  el.addEventListener('pointercancel', up);
  el.addEventListener('pointerleave', up);
}

setupButtonEvents('magic-left-click', 'L');
setupButtonEvents('magic-right-click', 'R');
setupButtonEvents('magic-mid-click', 'M');
setupButtonEvents('btn-left-click', 'L');
setupButtonEvents('btn-right-click', 'R');
setupButtonEvents('btn-mid-click', 'M');

function sendScroll(steps) {
  send("S:" + (steps * 3));
  vibrate(15);
}

// -------------------------------------------------------------
// CONSOLE D-PAD (AUTO-REPEAT ON HOLD)
// -------------------------------------------------------------
function setupRepeatingDpad(elementId, keyCode) {
  const el = document.getElementById(elementId);
  if (!el) return;
  let initialTimer = null;
  let repeatInterval = null;

  const start = (e) => {
    e.preventDefault();
    sendKey(keyCode);
    vibrate(25);
    el.classList.add('active');

    initialTimer = setTimeout(() => {
      repeatInterval = setInterval(() => {
        sendKey(keyCode);
        vibrate(15);
      }, 75);
    }, 280);
  };

  const stop = (e) => {
    el.classList.remove('active');
    if (initialTimer) clearTimeout(initialTimer);
    if (repeatInterval) clearInterval(repeatInterval);
    initialTimer = null;
    repeatInterval = null;
  };

  el.addEventListener('pointerdown', start);
  el.addEventListener('pointerup', stop);
  el.addEventListener('pointercancel', stop);
  el.addEventListener('pointerleave', stop);
}

setupRepeatingDpad('dpad-btn-up', 'UP');
setupRepeatingDpad('dpad-btn-down', 'DOWN');
setupRepeatingDpad('dpad-btn-left', 'LEFT');
setupRepeatingDpad('dpad-btn-right', 'RIGHT');

// -------------------------------------------------------------
// TOUCHPAD POINTER TRACKING
// -------------------------------------------------------------
const pad = document.getElementById('trackpad');
let lastX = 0, lastY = 0;
let isTouching = false;
let touchStartTime = 0;
let totalMovement = 0;
let sensitivity = 1.6;

const sensSlider = document.getElementById('sens-slider');
const sensVal = document.getElementById('sens-val');
sensSlider.addEventListener('input', (e) => {
  sensitivity = parseFloat(e.target.value);
  sensVal.innerText = sensitivity.toFixed(1) + 'x';
});

pad.addEventListener('pointerdown', (e) => {
  pad.setPointerCapture(e.pointerId);
  isTouching = true;
  lastX = e.clientX;
  lastY = e.clientY;
  touchStartTime = Date.now();
  totalMovement = 0;
});

pad.addEventListener('pointermove', (e) => {
  if (!isTouching) return;
  const rawDx = (e.clientX - lastX) * sensitivity;
  const rawDy = (e.clientY - lastY) * sensitivity;
  lastX = e.clientX;
  lastY = e.clientY;

  totalMovement += Math.abs(rawDx) + Math.abs(rawDy);
  queueMouseMove(rawDx, rawDy);
});

function handlePointerEnd(e) {
  if (!isTouching) return;
  isTouching = false;
  const duration = Date.now() - touchStartTime;
  if (duration < 250 && totalMovement < 8) {
    send("C:L");
    vibrate(25);
  }
}
pad.addEventListener('pointerup', handlePointerEnd);
pad.addEventListener('pointercancel', handlePointerEnd);

pad.addEventListener('touchstart', (e) => {
  if (e.touches.length === 2) {
    send("C:R");
    vibrate(35);
  }
}, { passive: true });

// -------------------------------------------------------------
// KEYBOARD & INPUT FUNCTIONS
// -------------------------------------------------------------
function sendKey(k) {
  send("K:" + k);
  vibrate(20);
}

function sendCombo(c) {
  send("COMBO:" + c);
  vibrate(30);
}

function sendMedia(m) {
  send("MEDIA:" + m);
  vibrate(25);
}

const activeMods = { CTRL: false, ALT: false, SHIFT: false, WIN: false };
function toggleMod(mod) {
  activeMods[mod] = !activeMods[mod];
  const btn = document.getElementById('mod-' + mod);
  if (activeMods[mod]) {
    send("KD:" + mod);
    btn.classList.add('active');
  } else {
    send("KU:" + mod);
    btn.classList.remove('active');
  }
  vibrate(25);
}

const textInput = document.getElementById('text-input');
const liveTyping = document.getElementById('live-typing');

textInput.addEventListener('input', (e) => {
  if (!liveTyping.checked) return;
  if (e.inputType === 'deleteContentBackward') {
    sendKey('BACKSPACE');
  } else if (e.data) {
    send("T:" + e.data);
  }
});

textInput.addEventListener('keydown', (e) => {
  if (e.key === 'Enter') {
    sendKey('ENTER');
    if (liveTyping.checked) textInput.value = '';
  }
});

function sendInputText() {
  const val = textInput.value;
  if (val.length > 0) {
    send("T:" + val);
    textInput.value = '';
    vibrate(25);
  }
}
</script>
</body>
</html>"""

# 1. Write web_page.h
target_h = r"C:\Users\Gourav\Documents\Random Projects\ESP32_BLE_WiFi_Remote\src\web_page.h"
with open(target_h, "w", encoding="utf-8") as f:
    f.write('#pragma once\n\n#include <Arduino.h>\n\nconst char INDEX_HTML[] PROGMEM = R"rawliteral(' + html_content + ')rawliteral";\n')
print(f"Updated {target_h}")

# 2. Compress to web_page_gz.h
compressed = gzip.compress(html_content.encode("utf-8"), 9)
hex_bytes = [f"0x{b:02x}" for b in compressed]
lines = []
for i in range(0, len(hex_bytes), 16):
    lines.append("    " + ", ".join(hex_bytes[i:i+16]))

c_code = """#pragma once
#include <Arduino.h>

const uint32_t INDEX_HTML_GZ_LEN = """ + str(len(compressed)) + """;
const uint8_t INDEX_HTML_GZ[] PROGMEM = {
""" + ",\n".join(lines) + """
};
"""

target_gz = r"C:\Users\Gourav\Documents\Random Projects\ESP32_BLE_WiFi_Remote\src\web_page_gz.h"
with open(target_gz, "w", encoding="utf-8") as f:
    f.write(c_code)

print(f"Updated {target_gz}: {len(html_content)} bytes -> {len(compressed)} bytes (gzip)")
