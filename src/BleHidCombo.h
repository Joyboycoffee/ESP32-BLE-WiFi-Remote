#pragma once

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEHIDDevice.h>

// Standard HID Mouse buttons
#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MIDDLE 0x04

// Media / Consumer Control Keys
#define MEDIA_KEY_NEXT_TRACK     0x00B5
#define MEDIA_KEY_PREV_TRACK     0x00B6
#define MEDIA_KEY_STOP           0x00B7
#define MEDIA_KEY_PLAY_PAUSE     0x00CD
#define MEDIA_KEY_MUTE           0x00E2
#define MEDIA_KEY_VOLUME_UP      0x00E9
#define MEDIA_KEY_VOLUME_DOWN    0x00EA

// Keyboard Modifiers (bits)
#define KEY_MOD_LCTRL  0x01
#define KEY_MOD_LSHIFT 0x02
#define KEY_MOD_LALT   0x04
#define KEY_MOD_LMETA  0x08  // Windows / GUI / Super key
#define KEY_MOD_RCTRL  0x10
#define KEY_MOD_RSHIFT 0x20
#define KEY_MOD_RALT   0x40
#define KEY_MOD_RMETA  0x80

// Special USB HID Keyboard Usage Codes
#define HID_KEY_RETURN      0x28
#define HID_KEY_ESCAPE      0x29
#define HID_KEY_BACKSPACE   0x2A
#define HID_KEY_TAB         0x2B
#define HID_KEY_SPACE       0x2C
#define HID_KEY_CAPSLOCK    0x39
#define HID_KEY_F1          0x3A
#define HID_KEY_F2          0x3B
#define HID_KEY_F3          0x3C
#define HID_KEY_F4          0x3D
#define HID_KEY_F5          0x3E
#define HID_KEY_F6          0x3F
#define HID_KEY_F7          0x40
#define HID_KEY_F8          0x41
#define HID_KEY_F9          0x42
#define HID_KEY_F10         0x43
#define HID_KEY_F11         0x44
#define HID_KEY_F12         0x45
#define HID_KEY_PRINTSCREEN 0x46
#define HID_KEY_SCROLLLOCK  0x47
#define HID_KEY_PAUSE       0x48
#define HID_KEY_INSERT      0x49
#define HID_KEY_HOME        0x4A
#define HID_KEY_PAGEUP      0x4B
#define HID_KEY_DELETE      0x4C
#define HID_KEY_END         0x4D
#define HID_KEY_PAGEDOWN    0x4E
#define HID_KEY_RIGHT       0x4F
#define HID_KEY_LEFT        0x50
#define HID_KEY_DOWN        0x51
#define HID_KEY_UP          0x52

typedef void (*BleConnectionCallback)(bool connected);

class BleHidCombo : public NimBLEServerCallbacks {
public:
    BleHidCombo();

    bool begin(const char* deviceName = "ESP 32 C3", const char* manufacturer = "Espressif");
    bool isConnected() const { return m_connected; }
    void setConnectionCallback(BleConnectionCallback cb) { m_connectionCb = cb; }

    // Mouse methods
    void mouseMove(int8_t dx, int8_t dy);
    void mouseScroll(int8_t wheel);
    void mousePress(uint8_t buttons);
    void mouseRelease(uint8_t buttons);
    void mouseClick(uint8_t buttons);

    // Keyboard methods
    void sendKey(uint8_t hidCode, uint8_t modifiers = 0);
    void keyPress(uint8_t hidCode);
    void keyRelease(uint8_t hidCode);
    void modifierPress(uint8_t modBit);
    void modifierRelease(uint8_t modBit);
    void releaseAllKeys();

    // Type text / ASCII
    void typeChar(char c);
    void typeString(const String& str);

    // Consumer / Media keys
    void sendMediaKey(uint16_t mediaCode);

    // Callbacks from NimBLEServerCallbacks
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;

private:
    struct __attribute__((packed)) KeyboardReport {
        uint8_t modifiers;
        uint8_t reserved;
        uint8_t keys[6];
    };

    struct __attribute__((packed)) MouseReport {
        uint8_t buttons;
        int8_t x;
        int8_t y;
        int8_t wheel;
    };

    bool m_connected;
    NimBLEServer* m_server;
    NimBLEHIDDevice* m_hid;
    NimBLECharacteristic* m_inputKeyboard;
    NimBLECharacteristic* m_outputKeyboard;
    NimBLECharacteristic* m_inputMouse;
    NimBLECharacteristic* m_inputMedia;

    KeyboardReport m_keyReport;
    MouseReport m_mouseReport;
    BleConnectionCallback m_connectionCb;

    void sendKeyboardReport();
    void sendMouseReport();
};

extern BleHidCombo bleCombo;
