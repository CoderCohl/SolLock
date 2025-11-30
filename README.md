```text
███████╗ ██████╗ ██╗      ██╗      ██████╗  ██████╗██╗  ██╗
██╔════╝██╔═══██╗██║      ██║     ██╔═══██╗██╔════╝██║ ██╔╝
███████╗██║   ██║██║      ██║     ██║   ██║██║     █████╔╝ 
╚════██║██║   ██║██║      ██║     ██║   ██║██║     ██╔═██╗ 
███████║╚██████╔╝███████╗███████╗╚██████╔╝╚██████╗██║  ██╗
╚══════╝ ╚═════╝ ╚══════╝╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝
```

# SolLock — Solana NFC Access Control System

Watch me build it: https://youtu.be/Nb1tHa4NIRw

SolLock is a hardware + software project that links **Solana wallet identity** to **physical access hardware**.

Using an ESP32, a PN532 NFC reader, and a small backend service, SolLock can:

- Read a wallet address from an NFC tag or phone
- Send it securely to an API
- Verify whether that wallet is authorized (allowlist and/or NFT ownership)
- Energize a relay to unlock a door, safe, locker, or any physical device

---

## 1️⃣ Why SolLock?

Most physical access systems still rely on:

- Metal keys
- Static keycodes
- Easily cloned NFC cards

At the same time, crypto wallets already provide:

- Strong cryptographic identity
- Easy revocation (move funds, change wallet)
- Programmatic policies (NFTs, tokens, DAOs)

SolLock is an experiment that asks:

> What if the same wallet that signs transactions could also unlock your door?

By wiring cheap hardware to a Solana-aware backend, SolLock turns wallets into real-world keys.

---

## 2️⃣ High-Level Architecture

```text
[ Wallet / NFC Tag ]
          │
          │ NFC scan
          ▼
[ ESP32 + PN532 ] --HTTPS--> [ SolLock Backend ] --RPC--> [ Solana ]
          │
          │ Relay control
          ▼
        [ Lock ]
```

- The ESP32 never talks directly to Solana.
- The backend handles all RPC calls and authorization logic.
- The lock only needs to know: **authorized: yes or no**.

---

## 3️⃣ Components

### Hardware

- ESP32 DevKit
- PN532 NFC module (I²C mode)
- Relay or MOSFET driver
- Door strike / solenoid / servo (any 5–12V lock hardware)
- Status LED (optional buzzer)

### Firmware

- Written in C++ using the Arduino framework
- Responsibilities:
  - Connect to Wi-Fi
  - Poll the PN532 for NFC tags
  - Derive or read a Solana wallet address from the tag
  - Call the `/auth` endpoint on the backend
  - Drive the relay + LED based on the response

### Backend

- Node.js + TypeScript + Express
- Responsibilities:
  - Authenticate incoming lock requests with an API key
  - Check wallet against an allowlist
  - Optionally verify ownership of a required NFT using `@solana/web3.js`
  - Return `{ ok: true | false }` to the lock

---

## Firmware Overview

- Located in `firmware/`
- Built with PlatformIO for ESP32
- Uses:
  - `Adafruit_PN532` for NFC
  - `ArduinoJson` for JSON encoding/decoding
- Configuration values (Wi-Fi, backend URL, lock ID, API key) are defined at the top of `src/main.cpp`.

Main loop responsibilities:

1. Wait for NFC tag.
2. Read tag UID (demo) or NDEF payload (real setup).
3. Build a pseudo wallet ID or extract a real wallet address.
4. POST `{ walletAddress, lockId }` to the `/auth` endpoint.
5. If authorized:
   - Activate relay for `UNLOCK_MS` milliseconds.
   - Blink LED with a “success” pattern.
6. If denied:
   - Keep lock closed.
   - Blink LED with a “denied” pattern.

---

## Backend Overview

- Located in `backend/`
- Uses Express with a small set of endpoints:

### `POST /auth`

- Headers:
  - `x-lock-api-key: <ADMIN_API_KEY>`
- Body:
  ```json
  {
    "walletAddress": "WALLET_04AABBCCDD",
    "lockId": "front-door-1"
  }
  ```
- Logic:
  - Confirm the API key matches `ADMIN_API_KEY`.
  - Look up `lockId` in an in-memory allowlist.
  - Optionally check whether `walletAddress` holds a configured NFT mint.
  - Return `{ ok: true }` if authorized, `{ ok: false }` otherwise.

---

## Web Demo

A minimal React-based dashboard is included in `web-demo/`:

- Allows you to:
  - Enter a wallet address
  - Enter a lock ID
  - Simulate what the lock would receive from `/auth`
- Useful for:
  - Testing policies
  - Debugging backend responses
  - Demoing SolLock without hardware plugged in

---

## 📚 Docs

Detailed docs live in the `docs/` folder:

- `01_overview.md` — Concept and user journey  
- `02_architecture.md` — Hardware + software diagram  
- `03_firmware.md` — NFC + ESP32 details  
- `04_backend.md` — API contract and Solana integration  
- `05_security_model.md` — Threat model and limitations  

---

## Security Notes

SolLock is **experimental** and should not be treated as a complete commercial security solution.

Important considerations:

- NFC tags can be cloned if used as static identifiers.
- Backend compromise could lead to unauthorized unlocks.
- Replay protection and challenge–response schemes are not fully implemented.
- Always assume that physical hardware can be tampered with.

Use SolLock as:

- A prototype
- A hackathon project
- A proof-of-concept for wallet-as-key ideas

Do not rely on it as your only layer of protection for high-value assets.

---

## Getting Started (Quick Summary)

1. Flash the ESP32 with the firmware in `firmware/`.
2. Run the backend with:
   ```bash
   cd backend
   npm install
   npm run dev
   ```
3. Configure your Wi-Fi, backend URL, and API key in `firmware/src/main.cpp`.
4. Power the lock hardware (relay + strike/solenoid).
5. Hold a compatible NFC tag or phone near the reader.
6. Watch the debug output and the lock behavior.

---

If you build on top of SolLock, improve the hardware, or harden the security, feel free to fork and extend it.
