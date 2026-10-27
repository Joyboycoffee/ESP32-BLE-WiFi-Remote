#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <esp_wifi.h>
#include <esp_netif.h>
extern "C" {
#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"
}
#include "BleHidCombo.h"
#include "web_page_gz.h"
#include <Update.h>
#include "ota_page.h"

// Network Configuration
static const char* AP_SSID = "ESP 32 C3";
static const IPAddress AP_IP(192, 168, 4, 1);
static const IPAddress AP_NETMASK(255, 255, 255, 0);
static const byte DNS_PORT = 53;

DNSServer dnsServer;
WebServer server(80);
WebSocketsServer webSocket(81);

// Forward declarations
void handleRoot();
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void processCommand(const String& cmd);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n============================================");
    Serial.println("         ESP 32 C3 Initializing             ");
    Serial.println("============================================");

    // 1. Wi-Fi Event Callbacks for diagnostics
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
            Serial.printf("[WiFi-AP] Client Associated! MAC: %02X:%02X:%02X:%02X:%02X:%02X, AID: %d\n",
                info.wifi_ap_staconnected.mac[0], info.wifi_ap_staconnected.mac[1],
                info.wifi_ap_staconnected.mac[2], info.wifi_ap_staconnected.mac[3],
                info.wifi_ap_staconnected.mac[4], info.wifi_ap_staconnected.mac[5],
                info.wifi_ap_staconnected.aid);
        } else if (event == ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED) {
            Serial.printf("[WiFi-AP] DHCP IP Assigned: %s\n", IPAddress(info.wifi_ap_staipassigned.ip.addr).toString().c_str());
        } else if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
            Serial.printf("[WiFi-AP] Client Disconnected! AID: %d. Resetting DHCPS lease table...\n", info.wifi_ap_stadisconnected.aid);
            esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
            if (ap_netif) {
                esp_netif_dhcps_stop(ap_netif);
                esp_netif_dhcps_start(ap_netif);
            }
        }
    });

    // 2. Initialize Wi-Fi SoftAP
    WiFi.mode(WIFI_AP);
    WiFi.setSleep(false);
    WiFi.softAPConfig(AP_IP, AP_IP, AP_NETMASK);
    bool apStarted = WiFi.softAP(AP_SSID, nullptr, 6, 0, 4);
    WiFi.setTxPower(WIFI_POWER_11dBm);

    if (apStarted) {
        Serial.printf("[WiFi] SoftAP started successfully!\n");
        Serial.printf("[WiFi] SSID: %s (Open Network)\n", AP_SSID);
        Serial.printf("[WiFi] IP Address: %s\n", WiFi.softAPIP().toString().c_str());
        Serial.printf("[WiFi] TX Power set to: %d (11dBm mode to eliminate crystal pulling)\n", WiFi.getTxPower());
    } else {
        Serial.println("[WiFi] ERROR: Failed to start SoftAP!");
    }

    // 4. Initialize mDNS (http://esp32.local)
    if (MDNS.begin("esp32")) {
        MDNS.addService("http", "tcp", 80);
        Serial.println("[mDNS] Responder started: http://esp32.local");
    }

    // 5. Initialize BLE HID Keyboard & Mouse
    bleCombo.setConnectionCallback([](bool connected) {
        String msg = String("STATUS:BLE=") + (connected ? "1" : "0");
        webSocket.broadcastTXT(msg);
        Serial.printf("[BLE] Broadcast status: %s\n", msg.c_str());
    });
    bleCombo.begin("ESP 32 C3", "Espressif");

    // 6. Initialize DNS Server for Captive Portal
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", AP_IP);
    Serial.println("[DNS] Captive Portal DNS server started on port 53.");

    // 7. Initialize WebServer routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/index.html", HTTP_GET, handleRoot);

    // OTA Firmware Update Endpoints
    server.on("/update", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send_P(200, "text/html", OTA_PAGE_HTML);
    });

    server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        if (!Update.hasError()) {
            delay(500);
            ESP.restart();
        }
    }, []() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
            Serial.printf("[OTA] Update Start: %s\n", upload.filename.c_str());
            if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
            if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_END) {
            if (Update.end(true)) {
                Serial.printf("[OTA] Update Success: %u bytes written\n", upload.totalSize);
            } else {
                Update.printError(Serial);
            }
        } else if (upload.status == UPLOAD_FILE_ABORTED) {
            Update.end();
            Serial.println("[OTA] Update was aborted");
        }
    });
    
    // Connectivity check endpoints
    // Android / Chrome: Returning 204 tells the phone Wi-Fi is good, so it won't route through cellular
    server.on("/generate_204", HTTP_GET, []() {
        server.send(204, "text/plain", "");
    });
    server.on("/gen_204", HTTP_GET, []() {
        server.send(204, "text/plain", "");
    });
    // Apple iOS
    server.on("/hotspot-detect.html", HTTP_GET, []() {
        server.send(200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>");
    });
    server.on("/canonical.html", HTTP_GET, []() {
        server.send(200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>");
    });
    // Windows NCSI
    server.on("/connecttest.txt", HTTP_GET, []() {
        server.send(200, "text/plain", "Microsoft Connect Test");
    });
    server.on("/ncsi.txt", HTTP_GET, []() {
        server.send(200, "text/plain", "Microsoft NCSI");
    });

    // Smart 404 / Redirect handler
    server.onNotFound([]() {
        String uri = server.uri();
        if (uri.indexOf("generate_204") >= 0 || uri.indexOf("gen_204") >= 0) {
            server.send(204, "text/plain", "");
            return;
        }
        if (uri.indexOf("hotspot-detect") >= 0 || uri.indexOf("canonical.html") >= 0) {
            server.send(200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>");
            return;
        }
        if (uri.endsWith(".ico") || uri.endsWith(".png") || uri.endsWith(".jpg") || 
            uri.endsWith(".json") || uri.endsWith(".xml") || uri.endsWith(".map")) {
            server.send(404, "text/plain", "Not Found");
            return;
        }
        String host = server.hostHeader();
        if (host.length() > 0 && host != "192.168.4.1" && host != "esp32.local") {
            server.sendHeader("Location", "http://192.168.4.1/", true);
            server.send(302, "text/plain", "Redirecting to ESP 32 C3...");
            return;
        }
        server.send(404, "text/plain", "Not Found");
    });
    server.begin();
    Serial.println("[HTTP] Web Server started on port 80.");

    // 8. Initialize WebSockets Server
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
    Serial.println("[WS] WebSocket Server started on port 81.");
    Serial.println("============================================");
    Serial.println("Ready! Connect your phone to Wi-Fi 'ESP 32 C3'");
    Serial.println("Open http://192.168.4.1 or http://esp32.local");
    Serial.println("And pair your PC to Bluetooth 'ESP 32 C3'");
    Serial.println("============================================\n");
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
    webSocket.loop();

    static unsigned long lastHb = 0;
    if (millis() - lastHb > 2000) {
        lastHb = millis();
        Serial.printf("[Heartbeat] Uptime: %lu s, Free Heap: %u, Clients: %d\n", 
            millis() / 1000, ESP.getFreeHeap(), WiFi.softAPgetStationNum());
    }
}

void handleRoot() {
    server.sendHeader("Content-Encoding", "gzip");
    server.send_P(200, "text/html", (PGM_P)INDEX_HTML_GZ, INDEX_HTML_GZ_LEN);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[WS] Client #%u connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
            // Send current BLE status to newly connected client
            String statusMsg = String("STATUS:BLE=") + (bleCombo.isConnected() ? "1" : "0");
            webSocket.sendTXT(num, statusMsg);
            break;
        }
        case WStype_DISCONNECTED: {
            Serial.printf("[WS] Client #%u disconnected\n", num);
            break;
        }
        case WStype_TEXT: {
            if (length > 0) {
                String cmd = String((char*)payload).substring(0, length);
                processCommand(cmd);
            }
            break;
        }
        default:
            break;
    }
}

void processCommand(const String& cmd) {
    // -------------------------------------------------------------
    // Mouse Move: M:<dx>:<dy>
    // -------------------------------------------------------------
    if (cmd.startsWith("M:")) {
        int firstColon = cmd.indexOf(':');
        int secondColon = cmd.indexOf(':', firstColon + 1);
        if (secondColon > firstColon) {
            int dx = cmd.substring(firstColon + 1, secondColon).toInt();
            int dy = cmd.substring(secondColon + 1).toInt();
            bleCombo.mouseMove((int8_t)constrain(dx, -127, 127), (int8_t)constrain(dy, -127, 127));
        }
    }
    // -------------------------------------------------------------
    // Mouse Click: C:<L|R|M>
    // -------------------------------------------------------------
    else if (cmd.startsWith("C:")) {
        char btn = cmd.charAt(2);
        if (btn == 'L') bleCombo.mouseClick(MOUSE_BUTTON_LEFT);
        else if (btn == 'R') bleCombo.mouseClick(MOUSE_BUTTON_RIGHT);
        else if (btn == 'M') bleCombo.mouseClick(MOUSE_BUTTON_MIDDLE);
    }
    // -------------------------------------------------------------
    // Mouse Down (Drag): MD:<L|R|M>
    // -------------------------------------------------------------
    else if (cmd.startsWith("MD:")) {
        char btn = cmd.charAt(3);
        if (btn == 'L') bleCombo.mousePress(MOUSE_BUTTON_LEFT);
        else if (btn == 'R') bleCombo.mousePress(MOUSE_BUTTON_RIGHT);
        else if (btn == 'M') bleCombo.mousePress(MOUSE_BUTTON_MIDDLE);
    }
    // -------------------------------------------------------------
    // Mouse Up: MU:<L|R|M>
    // -------------------------------------------------------------
    else if (cmd.startsWith("MU:")) {
        char btn = cmd.charAt(3);
        if (btn == 'L') bleCombo.mouseRelease(MOUSE_BUTTON_LEFT);
        else if (btn == 'R') bleCombo.mouseRelease(MOUSE_BUTTON_RIGHT);
        else if (btn == 'M') bleCombo.mouseRelease(MOUSE_BUTTON_MIDDLE);
    }
    // -------------------------------------------------------------
    // Scroll Wheel: S:<wheel>
    // -------------------------------------------------------------
    else if (cmd.startsWith("S:")) {
        int steps = cmd.substring(2).toInt();
        bleCombo.mouseScroll((int8_t)constrain(steps, -127, 127));
    }
    // -------------------------------------------------------------
    // Type Text: T:<string>
    // -------------------------------------------------------------
    else if (cmd.startsWith("T:")) {
        String text = cmd.substring(2);
        bleCombo.typeString(text);
    }
    // -------------------------------------------------------------
    // Special Key Tap: K:<KEY_NAME>
    // -------------------------------------------------------------
    else if (cmd.startsWith("K:")) {
        String key = cmd.substring(2);
        if (key == "ENTER")          bleCombo.sendKey(HID_KEY_RETURN);
        else if (key == "BACKSPACE") bleCombo.sendKey(HID_KEY_BACKSPACE);
        else if (key == "TAB")       bleCombo.sendKey(HID_KEY_TAB);
        else if (key == "ESC")       bleCombo.sendKey(HID_KEY_ESCAPE);
        else if (key == "SPACE")     bleCombo.sendKey(HID_KEY_SPACE);
        else if (key == "DELETE")    bleCombo.sendKey(HID_KEY_DELETE);
        else if (key == "HOME")      bleCombo.sendKey(HID_KEY_HOME);
        else if (key == "END")       bleCombo.sendKey(HID_KEY_END);
        else if (key == "PAGEUP")    bleCombo.sendKey(HID_KEY_PAGEUP);
        else if (key == "PAGEDOWN")  bleCombo.sendKey(HID_KEY_PAGEDOWN);
        else if (key == "UP")        bleCombo.sendKey(HID_KEY_UP);
        else if (key == "DOWN")      bleCombo.sendKey(HID_KEY_DOWN);
        else if (key == "LEFT")      bleCombo.sendKey(HID_KEY_LEFT);
        else if (key == "RIGHT")     bleCombo.sendKey(HID_KEY_RIGHT);
        else if (key.startsWith("F")) {
            int fnum = key.substring(1).toInt();
            if (fnum >= 1 && fnum <= 12) {
                bleCombo.sendKey(HID_KEY_F1 + (fnum - 1));
            }
        }
    }
    // -------------------------------------------------------------
    // Modifier Key Down: KD:<MOD>
    // -------------------------------------------------------------
    else if (cmd.startsWith("KD:")) {
        String mod = cmd.substring(3);
        if (mod == "CTRL")       bleCombo.modifierPress(KEY_MOD_LCTRL);
        else if (mod == "ALT")   bleCombo.modifierPress(KEY_MOD_LALT);
        else if (mod == "SHIFT") bleCombo.modifierPress(KEY_MOD_LSHIFT);
        else if (mod == "WIN")   bleCombo.modifierPress(KEY_MOD_LMETA);
    }
    // -------------------------------------------------------------
    // Modifier Key Up: KU:<MOD>
    // -------------------------------------------------------------
    else if (cmd.startsWith("KU:")) {
        String mod = cmd.substring(3);
        if (mod == "CTRL")       bleCombo.modifierRelease(KEY_MOD_LCTRL);
        else if (mod == "ALT")   bleCombo.modifierRelease(KEY_MOD_LALT);
        else if (mod == "SHIFT") bleCombo.modifierRelease(KEY_MOD_LSHIFT);
        else if (mod == "WIN")   bleCombo.modifierRelease(KEY_MOD_LMETA);
    }
    // -------------------------------------------------------------
    // Hotkey Combos: COMBO:<NAME>
    // -------------------------------------------------------------
    else if (cmd.startsWith("COMBO:")) {
        String combo = cmd.substring(6);
        if (combo == "COPY")             bleCombo.sendKey(0x06, KEY_MOD_LCTRL); // 'c'
        else if (combo == "PASTE")       bleCombo.sendKey(0x19, KEY_MOD_LCTRL); // 'v'
        else if (combo == "CUT")         bleCombo.sendKey(0x1B, KEY_MOD_LCTRL); // 'x'
        else if (combo == "SELECT_ALL")  bleCombo.sendKey(0x04, KEY_MOD_LCTRL); // 'a'
        else if (combo == "UNDO")        bleCombo.sendKey(0x1D, KEY_MOD_LCTRL); // 'z'
        else if (combo == "ALT_TAB")     bleCombo.sendKey(HID_KEY_TAB, KEY_MOD_LALT);
        else if (combo == "WIN_D")       bleCombo.sendKey(0x07, KEY_MOD_LMETA); // 'd'
        else if (combo == "WIN_R")       bleCombo.sendKey(0x15, KEY_MOD_LMETA); // 'r'
        else if (combo == "LOCK")        bleCombo.sendKey(0x0F, KEY_MOD_LMETA); // 'l'
    }
    // -------------------------------------------------------------
    // Media / Consumer Control: MEDIA:<ACTION>
    // -------------------------------------------------------------
    else if (cmd.startsWith("MEDIA:")) {
        String action = cmd.substring(6);
        if (action == "VOL_UP")          bleCombo.sendMediaKey(MEDIA_KEY_VOLUME_UP);
        else if (action == "VOL_DOWN")   bleCombo.sendMediaKey(MEDIA_KEY_VOLUME_DOWN);
        else if (action == "MUTE")       bleCombo.sendMediaKey(MEDIA_KEY_MUTE);
        else if (action == "PLAY_PAUSE") bleCombo.sendMediaKey(MEDIA_KEY_PLAY_PAUSE);
        else if (action == "NEXT")       bleCombo.sendMediaKey(MEDIA_KEY_NEXT_TRACK);
        else if (action == "PREV")       bleCombo.sendMediaKey(MEDIA_KEY_PREV_TRACK);
    }
}
