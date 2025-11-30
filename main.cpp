#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

// --- CONFIG ------------------------------------------------------------------

const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* BACKEND_HOST  = "your-backend.local";  // or IP / domain
const int   BACKEND_PORT  = 3000;
const char* BACKEND_PATH  = "/auth";
const char* LOCK_API_KEY  = "super-secret-lock-key";
const char* LOCK_ID       = "front-door-1";

// GPIO pins
const int RELAY_PIN = 5;
const int LED_PIN   = 2;

const unsigned long UNLOCK_MS = 3000;

// I2C pins for PN532 (depends on your board)
#define PN532_SDA 21
#define PN532_SCL 22

// --- GLOBALS -----------------------------------------------------------------

TwoWire pn532Wire = TwoWire(0);
Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);

String lastWallet = "";
unsigned long lastAuthMs = 0;

// --- HELPERS -----------------------------------------------------------------

void setLED(uint8_t state) {
  digitalWrite(LED_PIN, state);
}

void blinkLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    setLED(HIGH);
    delay(delayMs);
    setLED(LOW);
    delay(delayMs);
  }
}

void unlockRelay() {
  digitalWrite(RELAY_PIN, HIGH);
  delay(UNLOCK_MS);
  digitalWrite(RELAY_PIN, LOW);
}

void connectWiFi() {
  Serial.printf("[WiFi] Connecting to %s\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 30) {
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] Connected, IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("[WiFi] Failed to connect.");
  }
}

bool nfcReaderPresent() {
  return true; // placeholder
}

String readWalletAddressFromTag() {
  boolean success;
  uint8_t uid[7];
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);

  if (!success) {
    return "";
  }

  Serial.print("[NFC] Card detected. UID: ");
  for (uint8_t i = 0; i < uidLength; i++) {
    Serial.print(uid[i], HEX); Serial.print(" ");
  }
  Serial.println();

  char buf[64];
  String pseudoWallet = "WALLET_";
  for (uint8_t i = 0; i < uidLength; i++) {
    sprintf(buf, "%02X", uid[i]);
    pseudoWallet += buf;
  }

  return pseudoWallet;
}

bool authorizeWallet(const String& walletAddress) {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  WiFiClientSecure client;
  client.setInsecure(); // demo only

  Serial.printf("[NET] Connecting to %s:%d\n", BACKEND_HOST, BACKEND_PORT);
  if (!client.connect(BACKEND_HOST, BACKEND_PORT)) {
    Serial.println("[NET] Connection failed");
    return false;
  }

  StaticJsonDocument<256> doc;
  doc["walletAddress"] = walletAddress;
  doc["lockId"] = LOCK_ID;
  String body;
  serializeJson(doc, body);

  String request = String("POST ") + BACKEND_PATH + " HTTP/1.1\r\n" +
                  "Host: " + BACKEND_HOST + "\r\n" +
                  "Content-Type: application/json\r\n" +
                  "x-lock-api-key: " + LOCK_API_KEY + "\r\n" +
                  "Content-Length: " + body.length() + "\r\n" +
                  "Connection: close\r\n\r\n" +
                  body;

  client.print(request);

  String response;
  while (client.connected() || client.available()) {
    String line = client.readStringUntil('\n');
    response += line + "\n";
  }

  int jsonIndex = response.indexOf("\r\n\r\n");
  if (jsonIndex < 0) {
    Serial.println("[NET] Invalid HTTP response");
    return false;
  }

  String json = response.substring(jsonIndex + 4);
  StaticJsonDocument<256> resDoc;
  auto err = deserializeJson(resDoc, json);
  if (err) {
    Serial.println("[NET] Failed to parse JSON response");
    return false;
  }

  bool ok = resDoc["ok"] | false;
  return ok;
}

// --- SETUP & LOOP ------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  setLED(LOW);

  Serial.println("[BOOT] SolLock firmware starting...");

  connectWiFi();

  pn532Wire.begin(PN532_SDA, PN532_SCL);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("[NFC] Didn't find PN53x board");
    while (1) {
      blinkLED(1, 250);
    }
  }

  Serial.print("[NFC] PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  nfc.SAMConfig();

  Serial.println("[NFC] Waiting for NFC card...");
}

void loop() {
  String walletAddress = readWalletAddressFromTag();
  if (walletAddress.length() == 0) {
    delay(100);
    return;
  }

  unsigned long now = millis();
  if (walletAddress == lastWallet && (now - lastAuthMs) < 2000) {
    return;
  }

  Serial.printf("[INFO] Wallet candidate: %s\n", walletAddress.c_str());

  setLED(HIGH);
  bool allowed = authorizeWallet(walletAddress);
  setLED(LOW);

  if (allowed) {
    Serial.println("[ACCESS] ✅ Access granted");
    unlockRelay();
    blinkLED(2, 150);
  } else {
    Serial.println("[ACCESS] ❌ Access denied");
    blinkLED(4, 150);
  }

  lastWallet = walletAddress;
  lastAuthMs = now;
}
