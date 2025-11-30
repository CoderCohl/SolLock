# Firmware Details (ESP32)

The firmware is implemented in **C++ using the Arduino framework**, built with PlatformIO.

## NFC Wallet Encoding

For simplicity, we assume:

- The NFC tag contains a **text NDEF record** whose payload is the Solana wallet address.
- Example: `4N9KcD8oQU...` (base58)

Alternative options (future):

- Wallet address encoded in a URI (`solana:4N9KcD8oQU...`)
- Tag UID mapped to wallet address in the backend database

## Core Loop Responsibilities

1. Poll PN532 for NFC cards.
2. When detected, read NDEF.
3. Parse wallet address from payload.
4. Call `authorizeWallet(walletAddress)` via HTTPS.
5. If `true`, energize relay for `UNLOCK_MS` milliseconds.
6. Blink LED according to result.

---

## WiFi & Config

WiFi credentials and backend URL are defined at the top of `main.cpp`:

```cpp
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* BACKEND_HOST  = "your-backend.local";
const int   BACKEND_PORT  = 3000;
const char* BACKEND_PATH  = "/auth";
const char* LOCK_API_KEY  = "super-secret-lock-key"; // must match backend ADMIN_API_KEY
const char* LOCK_ID       = "front-door-1";
```

See the full source in [`firmware/src/main.cpp`](../firmware/src/main.cpp).
