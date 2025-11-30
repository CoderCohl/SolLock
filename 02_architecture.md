# Architecture

## Hardware

- **MCU**: ESP32 DevKit
- **NFC Reader**: PN532 (I²C mode)
- **Lock Driver**: Relay module or MOSFET controlling:
  - door strike
  - solenoid
  - servo motor
- **Indicators**:
  - RGB LED (status color)
  - optional buzzer

## Software Components

1. **Firmware (C++ / Arduino)**  
   Runs on ESP32. Responsibilities:
   - Connect to WiFi
   - Detect NFC tags / NDEF records
   - Extract wallet address
   - Call backend over HTTPS
   - Drive lock & status LED

2. **Backend API (Node + TypeScript)**  
   Responsibilities:
   - Auth lock requests using `ADMIN_API_KEY`
   - Check wallet vs allowlist
   - (Optional) Query Solana RPC
   - Apply policy for each `lockId`
   - Log events (future extension)

3. **Web UI (React)**  
   Responsibilities:
   - Admin panel to:
     - register locks
     - manage allowlists
     - define NFT rules
     - view logs

4. **Solana Layer (via `@solana/web3.js`)**  
   Responsibilities:
   - Fetch NFT holdings
   - Verify token accounts
   - Potentially verify signatures for more advanced schemes

---

## Data Flow

```text
NFC Tag (wallet) 
   ↓
ESP32 + PN532
   ↓ HTTPS (POST)
Backend /auth
   ↓ RPC
Solana Network
```

- The **lock never talks directly to Solana**.
- The backend is the “translator” between hardware and chain.
