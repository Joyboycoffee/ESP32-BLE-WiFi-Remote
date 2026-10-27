#include "BleHidCombo.h"

BleHidCombo bleCombo;

namespace {

// Combined HID Report Descriptor: Keyboard (ID 1), Mouse (ID 2), Media (ID 3)
static const uint8_t hidReportMap[] = {
    // --------------------------------------------------------
    // Keyboard Report (ID 1)
    // --------------------------------------------------------
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xA1, 0x01,                    // COLLECTION (Application)
    0x85, 0x01,                    //   REPORT_ID (1)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard/Keypad)
    0x19, 0xE0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xE7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs) - Modifiers
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs) - Reserved byte
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs) - LEDs
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x01,                    //   OUTPUT (Cnst,Ary,Abs) - LED padding
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x73,                    //   LOGICAL_MAXIMUM (115)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard/Keypad)
    0x19, 0x00,                    //   USAGE_MINIMUM (0)
    0x29, 0x73,                    //   USAGE_MAXIMUM (115)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs) - Key array (6 keys)
    0xC0,                          // END_COLLECTION

    // --------------------------------------------------------
    // Mouse Report (ID 2)
    // --------------------------------------------------------
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xA1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //   REPORT_ID (2)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xA1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs) - Left, Right, Middle
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x01,                    //     INPUT (Cnst,Ary,Abs) - Padding
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel) - X, Y, Wheel
    0xC0,                          //   END_COLLECTION
    0xC0,                          // END_COLLECTION

    // --------------------------------------------------------
    // Media / Consumer Control Report (ID 3)
    // --------------------------------------------------------
    0x05, 0x0C,                    // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                    // USAGE (Consumer Control)
    0xA1, 0x01,                    // COLLECTION (Application)
    0x85, 0x03,                    //   REPORT_ID (3)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xFF, 0x03,              //   LOGICAL_MAXIMUM (1023)
    0x19, 0x00,                    //   USAGE_MINIMUM (0)
    0x2A, 0xFF, 0x03,              //   USAGE_MAXIMUM (1023)
    0x75, 0x10,                    //   REPORT_SIZE (16)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xC0                           // END_COLLECTION
};

// ASCII to HID Usage Code & Shift Map (index 0..127)
// High bit 0x80 indicates Shift modifier required
static const uint8_t asciiToHidMap[128] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x2A,       // 0x08: Backspace ('\b')
    0x2B,       // 0x09: Tab ('\t')
    0x28,       // 0x0A: Linefeed ('\n')
    0x00, 0x00,
    0x28,       // 0x0D: Return ('\r')
    0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, // 0x1B is ESC
    0x2C,       // 0x20: ' ' (Space)
    0x1E|0x80,  // 0x21: '!'
    0x34|0x80,  // 0x22: '"'
    0x20|0x80,  // 0x23: '#'
    0x21|0x80,  // 0x24: '$'
    0x22|0x80,  // 0x25: '%'
    0x24|0x80,  // 0x26: '&'
    0x34,       // 0x27: '\''
    0x26|0x80,  // 0x28: '('
    0x27|0x80,  // 0x29: ')'
    0x25|0x80,  // 0x2A: '*'
    0x2E|0x80,  // 0x2B: '+'
    0x36,       // 0x2C: ','
    0x2D,       // 0x2D: '-'
    0x37,       // 0x2E: '.'
    0x38,       // 0x2F: '/'
    0x27,       // 0x30: '0'
    0x1E,       // 0x31: '1'
    0x1F,       // 0x32: '2'
    0x20,       // 0x33: '3'
    0x21,       // 0x34: '4'
    0x22,       // 0x35: '5'
    0x23,       // 0x36: '6'
    0x24,       // 0x37: '7'
    0x25,       // 0x38: '8'
    0x26,       // 0x39: '9'
    0x33|0x80,  // 0x3A: ':'
    0x33,       // 0x3B: ';'
    0x36|0x80,  // 0x3C: '<'
    0x2E,       // 0x3D: '='
    0x37|0x80,  // 0x3E: '>'
    0x38|0x80,  // 0x3F: '?'
    0x1F|0x80,  // 0x40: '@'
    0x04|0x80,  // 0x41: 'A'
    0x05|0x80,  // 0x42: 'B'
    0x06|0x80,  // 0x43: 'C'
    0x07|0x80,  // 0x44: 'D'
    0x08|0x80,  // 0x45: 'E'
    0x09|0x80,  // 0x46: 'F'
    0x0A|0x80,  // 0x47: 'G'
    0x0B|0x80,  // 0x48: 'H'
    0x0C|0x80,  // 0x49: 'I'
    0x0D|0x80,  // 0x4A: 'J'
    0x0E|0x80,  // 0x4B: 'K'
    0x0F|0x80,  // 0x4C: 'L'
    0x10|0x80,  // 0x4D: 'M'
    0x11|0x80,  // 0x4E: 'N'
    0x12|0x80,  // 0x4F: 'O'
    0x13|0x80,  // 0x50: 'P'
    0x14|0x80,  // 0x51: 'Q'
    0x15|0x80,  // 0x52: 'R'
    0x16|0x80,  // 0x53: 'S'
    0x17|0x80,  // 0x54: 'T'
    0x18|0x80,  // 0x55: 'U'
    0x19|0x80,  // 0x56: 'V'
    0x1A|0x80,  // 0x57: 'W'
    0x1B|0x80,  // 0x58: 'X'
    0x1C|0x80,  // 0x59: 'Y'
    0x1D|0x80,  // 0x5A: 'Z'
    0x2F,       // 0x5B: '['
    0x31,       // 0x5C: '\\'
    0x30,       // 0x5D: ']'
    0x23|0x80,  // 0x5E: '^'
    0x2D|0x80,  // 0x5F: '_'
    0x35,       // 0x60: '`'
    0x04,       // 0x61: 'a'
    0x05,       // 0x62: 'b'
    0x06,       // 0x63: 'c'
    0x07,       // 0x64: 'd'
    0x08,       // 0x65: 'e'
    0x09,       // 0x66: 'f'
    0x0A,       // 0x67: 'g'
    0x0B,       // 0x68: 'h'
    0x0C,       // 0x69: 'i'
    0x0D,       // 0x6A: 'j'
    0x0E,       // 0x6B: 'k'
    0x0F,       // 0x6C: 'l'
    0x10,       // 0x6D: 'm'
    0x11,       // 0x6E: 'n'
    0x12,       // 0x6F: 'o'
    0x13,       // 0x70: 'p'
    0x14,       // 0x71: 'q'
    0x15,       // 0x72: 'r'
    0x16,       // 0x73: 's'
    0x17,       // 0x74: 't'
    0x18,       // 0x75: 'u'
    0x19,       // 0x76: 'v'
    0x1A,       // 0x77: 'w'
    0x1B,       // 0x78: 'x'
    0x1C,       // 0x79: 'y'
    0x1D,       // 0x7A: 'z'
    0x2F|0x80,  // 0x7B: '{'
    0x31|0x80,  // 0x7C: '|'
    0x30|0x80,  // 0x7D: '}'
    0x35|0x80,  // 0x7E: '~'
    0x4C        // 0x7F: DEL
};

} // namespace

BleHidCombo::BleHidCombo()
    : m_connected(false), m_server(nullptr), m_hid(nullptr),
      m_inputKeyboard(nullptr), m_outputKeyboard(nullptr),
      m_inputMouse(nullptr), m_inputMedia(nullptr),
      m_connectionCb(nullptr) {
    memset(&m_keyReport, 0, sizeof(m_keyReport));
    memset(&m_mouseReport, 0, sizeof(m_mouseReport));
}

bool BleHidCombo::begin(const char* deviceName, const char* manufacturer) {
    NimBLEDevice::init(deviceName);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    NimBLEDevice::setSecurityAuth(true, false, false);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    NimBLEDevice::setSecurityInitKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);
    NimBLEDevice::setSecurityRespKey(BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID);

    m_server = NimBLEDevice::createServer();
    m_server->setCallbacks(this);
    m_server->advertiseOnDisconnect(true);

    m_hid = new NimBLEHIDDevice(m_server);
    m_inputKeyboard  = m_hid->getInputReport(1);
    m_outputKeyboard = m_hid->getOutputReport(1);
    m_inputMouse     = m_hid->getInputReport(2);
    m_inputMedia     = m_hid->getInputReport(3);

    m_hid->setManufacturer(manufacturer);
    m_hid->setPnp(0x02, 0x046D, 0xB012, 0x0001); // Standard PnP ID
    m_hid->setHidInfo(0x00, 0x01);
    m_hid->setReportMap((uint8_t*)hidReportMap, sizeof(hidReportMap));
    m_hid->setBatteryLevel(100);

    // In NimBLE 2.x, m_server->start() starts all GATT services (HID, Battery, DeviceInfo)
    bool serverStarted = m_server->start();
    Serial.printf("[BLE] m_server->start() = %d\n", serverStarted);

    NimBLEAdvertising* adv = m_server->getAdvertising();
    adv->setConnectableMode(BLE_GAP_CONN_MODE_UND);
    adv->setDiscoverableMode(BLE_GAP_DISC_MODE_GEN);

    NimBLEAdvertisementData advData;
    advData.setFlags(BLE_HS_ADV_F_DISC_GEN);
    advData.setAppearance(HID_KEYBOARD); // Keyboard icon in Windows
    advData.addServiceUUID(NimBLEUUID((uint16_t)0x1812));
    advData.setName(deviceName);
    adv->setAdvertisementData(advData);

    NimBLEAdvertisementData scanData;
    scanData.setName(deviceName);
    scanData.setAppearance(HID_KEYBOARD);
    scanData.addServiceUUID(NimBLEUUID((uint16_t)0x1812));
    adv->setScanResponseData(scanData);
    adv->enableScanResponse(true);

    adv->setMinInterval(32); // 32 * 0.625ms = 20ms
    adv->setMaxInterval(64); // 64 * 0.625ms = 40ms
    bool started = adv->start();

    Serial.printf("[BLE] advData bytes (%d): ", (int)advData.getPayload().size());
    for (uint8_t b : advData.getPayload()) Serial.printf("%02X ", b);
    Serial.println();

    Serial.printf("[BLE] scanData bytes (%d): ", (int)scanData.getPayload().size());
    for (uint8_t b : scanData.getPayload()) Serial.printf("%02X ", b);
    Serial.println();

    return started;
}

void BleHidCombo::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
    m_connected = true;
    Serial.printf("[BLE] Device connected (handle=%d)! Requesting fast 7.5-15ms interval...\n", connInfo.getConnHandle());
    pServer->updateConnParams(connInfo.getConnHandle(), 6, 12, 0, 400);
    if (m_connectionCb) m_connectionCb(true);
}

void BleHidCombo::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
    m_connected = false;
    memset(&m_keyReport, 0, sizeof(m_keyReport));
    memset(&m_mouseReport, 0, sizeof(m_mouseReport));
    Serial.printf("[BLE] Device disconnected (reason=%d). Advertising restarted.\n", reason);
    pServer->getAdvertising()->start();
    if (m_connectionCb) m_connectionCb(false);
}

// -------------------------------------------------------------
// Mouse
// -------------------------------------------------------------
void BleHidCombo::sendMouseReport() {
    if (!m_connected || !m_inputMouse) return;
    m_inputMouse->setValue((uint8_t*)&m_mouseReport, sizeof(m_mouseReport));
    m_inputMouse->notify();
}

void BleHidCombo::mouseMove(int8_t dx, int8_t dy) {
    if (!m_connected || !m_inputMouse) return;
    m_mouseReport.x = dx;
    m_mouseReport.y = dy;
    m_mouseReport.wheel = 0;
    sendMouseReport();
    m_mouseReport.x = 0;
    m_mouseReport.y = 0;
}

void BleHidCombo::mouseScroll(int8_t wheel) {
    if (!m_connected || !m_inputMouse) return;
    m_mouseReport.x = 0;
    m_mouseReport.y = 0;
    m_mouseReport.wheel = wheel;
    sendMouseReport();
    m_mouseReport.wheel = 0;
}

void BleHidCombo::mousePress(uint8_t buttons) {
    if (!m_connected || !m_inputMouse) return;
    m_mouseReport.buttons |= buttons;
    m_mouseReport.x = 0;
    m_mouseReport.y = 0;
    m_mouseReport.wheel = 0;
    sendMouseReport();
}

void BleHidCombo::mouseRelease(uint8_t buttons) {
    if (!m_connected || !m_inputMouse) return;
    m_mouseReport.buttons &= ~buttons;
    m_mouseReport.x = 0;
    m_mouseReport.y = 0;
    m_mouseReport.wheel = 0;
    sendMouseReport();
}

void BleHidCombo::mouseClick(uint8_t buttons) {
    mousePress(buttons);
    delay(15);
    mouseRelease(buttons);
}

// -------------------------------------------------------------
// Keyboard
// -------------------------------------------------------------
void BleHidCombo::sendKeyboardReport() {
    if (!m_connected || !m_inputKeyboard) return;
    m_inputKeyboard->setValue((uint8_t*)&m_keyReport, sizeof(m_keyReport));
    m_inputKeyboard->notify();
}

void BleHidCombo::sendKey(uint8_t hidCode, uint8_t modifiers) {
    if (!m_connected || !m_inputKeyboard) return;
    m_keyReport.modifiers |= modifiers;
    m_keyReport.keys[0] = hidCode;
    sendKeyboardReport();
    delay(15);
    m_keyReport.keys[0] = 0;
    m_keyReport.modifiers &= ~modifiers;
    sendKeyboardReport();
}

void BleHidCombo::keyPress(uint8_t hidCode) {
    if (!m_connected || !m_inputKeyboard) return;
    for (int i = 0; i < 6; i++) {
        if (m_keyReport.keys[i] == 0) {
            m_keyReport.keys[i] = hidCode;
            break;
        }
    }
    sendKeyboardReport();
}

void BleHidCombo::keyRelease(uint8_t hidCode) {
    if (!m_connected || !m_inputKeyboard) return;
    for (int i = 0; i < 6; i++) {
        if (m_keyReport.keys[i] == hidCode) {
            m_keyReport.keys[i] = 0;
        }
    }
    sendKeyboardReport();
}

void BleHidCombo::modifierPress(uint8_t modBit) {
    m_keyReport.modifiers |= modBit;
    sendKeyboardReport();
}

void BleHidCombo::modifierRelease(uint8_t modBit) {
    m_keyReport.modifiers &= ~modBit;
    sendKeyboardReport();
}

void BleHidCombo::releaseAllKeys() {
    memset(&m_keyReport, 0, sizeof(m_keyReport));
    sendKeyboardReport();
}

void BleHidCombo::typeChar(char c) {
    if (!m_connected || !m_inputKeyboard) return;
    uint8_t uc = (uint8_t)c;
    if (uc > 127) return;

    uint8_t val = pgm_read_byte(&asciiToHidMap[uc]);
    if (val == 0) return;

    uint8_t hidCode = val & 0x7F;
    bool needsShift = (val & 0x80) != 0;

    uint8_t prevMods = m_keyReport.modifiers;
    if (needsShift) m_keyReport.modifiers |= KEY_MOD_LSHIFT;
    m_keyReport.keys[0] = hidCode;
    sendKeyboardReport();
    delay(10);

    m_keyReport.keys[0] = 0;
    m_keyReport.modifiers = prevMods;
    sendKeyboardReport();
    delay(10);
}

void BleHidCombo::typeString(const String& str) {
    for (size_t i = 0; i < str.length(); i++) {
        typeChar(str[i]);
    }
}

// -------------------------------------------------------------
// Media / Consumer Control
// -------------------------------------------------------------
void BleHidCombo::sendMediaKey(uint16_t mediaCode) {
    if (!m_connected || !m_inputMedia) return;
    uint8_t report[2];
    report[0] = (uint8_t)(mediaCode & 0xFF);
    report[1] = (uint8_t)((mediaCode >> 8) & 0xFF);

    m_inputMedia->setValue(report, sizeof(report));
    m_inputMedia->notify();
    delay(15);

    report[0] = 0;
    report[1] = 0;
    m_inputMedia->setValue(report, sizeof(report));
    m_inputMedia->notify();
}
