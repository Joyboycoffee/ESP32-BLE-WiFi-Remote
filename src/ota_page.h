#pragma once
#include <Arduino.h>

const char OTA_PAGE_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
<title>ESP 32 C3 - OTA Update</title>
<style>
:root {
  --bg-color: #080c14;
  --surface: #111827;
  --surface-alt: #1f293d;
  --border: #2e3c54;
  --accent: #38bdf8;
  --magic: #a855f7;
  --success: #22c55e;
  --danger: #ef4444;
  --text-main: #f8fafc;
  --text-dim: #94a3b8;
  --radius: 12px;
}
* { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; }
body {
  background: var(--bg-color);
  color: var(--text-main);
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  padding: 16px;
}
.card {
  background: var(--surface);
  border: 1px solid var(--border);
  border-radius: var(--radius);
  padding: 24px;
  max-width: 440px;
  width: 100%;
  box-shadow: 0 10px 25px rgba(0, 0, 0, 0.5);
  display: flex;
  flex-direction: column;
  gap: 16px;
}
.header {
  display: flex;
  align-items: center;
  gap: 12px;
}
.icon {
  font-size: 26px;
  background: rgba(56, 189, 248, 0.15);
  border: 1px solid var(--accent);
  border-radius: 10px;
  width: 48px;
  height: 48px;
  display: flex;
  align-items: center;
  justify-content: center;
}
h1 { font-size: 18px; font-weight: 700; color: #fff; }
.sub { font-size: 12px; color: var(--text-dim); }
.info-table {
  background: var(--surface-alt);
  border-radius: 8px;
  padding: 10px 14px;
  font-size: 12px;
  display: flex;
  flex-direction: column;
  gap: 6px;
}
.info-row { display: flex; justify-content: space-between; }
.info-label { color: var(--text-dim); }
.info-val { font-weight: 600; color: var(--accent); font-family: monospace; }
.file-drop {
  border: 2px dashed var(--border);
  border-radius: 10px;
  padding: 20px;
  text-align: center;
  cursor: pointer;
  transition: all 0.2s;
  background: rgba(31, 41, 61, 0.4);
}
.file-drop:hover { border-color: var(--accent); }
input[type="file"] { display: none; }
.btn {
  background: var(--accent);
  color: #080c14;
  border: none;
  border-radius: 8px;
  padding: 12px;
  font-weight: 700;
  font-size: 14px;
  cursor: pointer;
  width: 100%;
  transition: all 0.15s;
}
.btn:active { transform: scale(0.98); }
.btn:disabled { background: var(--border); color: var(--text-dim); cursor: not-allowed; }
.btn-back {
  background: transparent;
  border: 1px solid var(--border);
  color: var(--text-dim);
  margin-top: 6px;
}
.progress-bar-container {
  height: 10px;
  background: var(--surface-alt);
  border-radius: 999px;
  overflow: hidden;
  display: none;
}
.progress-bar {
  height: 100%;
  width: 0%;
  background: linear-gradient(90deg, var(--accent), var(--magic));
  transition: width 0.1s;
}
.status-msg {
  font-size: 12px;
  text-align: center;
  font-weight: 600;
  min-height: 18px;
}
</style>
</head>
<body>
<div class="card">
  <div class="header">
    <div class="icon">&#128225;</div>
    <div>
      <h1>Over-The-Air Update</h1>
      <div class="sub">ESP 32 C3 Firmware Flasher</div>
    </div>
  </div>

  <div class="info-table">
    <div class="info-row"><span class="info-label">Target Chip:</span><span class="info-val">ESP32-C3 (4MB)</span></div>
    <div class="info-row"><span class="info-label">Partition Scheme:</span><span class="info-val">Dual OTA (app0 / app1)</span></div>
    <div class="info-row"><span class="info-label">Status:</span><span class="info-val">Hotspot Online</span></div>
  </div>

  <div class="file-drop" onclick="document.getElementById('file-input').click()">
    <div style="font-size: 32px; margin-bottom: 6px;">&#128230;</div>
    <div id="file-name" style="font-weight: 600; font-size: 13px;">Click to select firmware.bin</div>
    <div style="font-size: 11px; color: var(--text-dim); margin-top: 4px;">Select the compiled application binary (.bin)</div>
    <input type="file" id="file-input" accept=".bin" onchange="onFileSelected(this)">
  </div>

  <div class="progress-bar-container" id="prog-wrap">
    <div class="progress-bar" id="prog-bar"></div>
  </div>

  <div class="status-msg" id="status-text"></div>

  <button class="btn" id="upload-btn" onclick="uploadFirmware()" disabled>Flash Firmware Over-The-Air</button>
  <button class="btn btn-back" onclick="window.location.href='/'">&larr; Return to Remote Portal</button>
</div>

<script>
let selectedFile = null;

function onFileSelected(input) {
  if (input.files && input.files[0]) {
    selectedFile = input.files[0];
    document.getElementById('file-name').innerText = selectedFile.name + ' (' + (selectedFile.size / 1024).toFixed(1) + ' KB)';
    document.getElementById('upload-btn').disabled = false;
    document.getElementById('status-text').innerText = 'Ready to flash. Keep phone near ESP32 during upload.';
    document.getElementById('status-text').style.color = '#38bdf8';
  }
}

function uploadFirmware() {
  if (!selectedFile) return;
  const btn = document.getElementById('upload-btn');
  const progWrap = document.getElementById('prog-wrap');
  const progBar = document.getElementById('prog-bar');
  const statusText = document.getElementById('status-text');

  btn.disabled = true;
  progWrap.style.display = 'block';
  statusText.innerText = 'Uploading firmware...';
  statusText.style.color = '#38bdf8';

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/update', true);

  xhr.upload.onprogress = (e) => {
    if (e.lengthComputable) {
      const pct = Math.round((e.loaded / e.total) * 100);
      progBar.style.width = pct + '%';
      statusText.innerText = 'Flashing: ' + pct + '% (' + (e.loaded / 1024).toFixed(0) + ' / ' + (e.total / 1024).toFixed(0) + ' KB)';
    }
  };

  xhr.onload = () => {
    if (xhr.status === 200 && xhr.responseText === 'OK') {
      progBar.style.width = '100%';
      statusText.innerText = 'Update Successful! Rebooting ESP32...';
      statusText.style.color = '#22c55e';
      setTimeout(() => {
        statusText.innerText = 'Reconnecting in 5 seconds...';
        setTimeout(() => { window.location.href = '/'; }, 5000);
      }, 2000);
    } else {
      statusText.innerText = 'Update failed: ' + xhr.responseText;
      statusText.style.color = '#ef4444';
      btn.disabled = false;
    }
  };

  xhr.onerror = () => {
    statusText.innerText = 'Network error during upload.';
    statusText.style.color = '#ef4444';
    btn.disabled = false;
  };

  const formData = new FormData();
  formData.append('update', selectedFile);
  xhr.send(formData);
}
</script>
</body>
</html>
)rawliteral";
