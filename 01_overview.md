# SolLock Overview

SolLock is an **NFC-enabled, Solana-aware lock** that bridges your crypto identity and physical access.

## User Journey

1. User writes or links their **Solana wallet address** to an NFC tag or phone wallet.
2. User approaches a door with a SolLock unit.
3. They **tap** the reader.
4. The ESP32 reads the tag, extracts the wallet address.
5. The firmware hits the SolLock backend `/auth` endpoint.
6. The backend checks:
   - Is this wallet in the allowlist?
   - Does it own a required NFT or token?
7. Backend responds `{ ok: true | false }`.
8. Firmware opens or keeps the lock closed accordingly.

## Example Use Cases

- **NFT-gated hacker house**  
  Only holders of your NFT can unlock the front door.

- **Shared hardware wallet safe**  
  Only a multi-sig’s signers can open the safe physically.

- **IRL club / “tokenized membership”**  
  Mint a membership NFT on Solana. Possession = literal door access.

- **Event wristbands**  
  NFC wristband with embedded wallet address → festival gates check membership on-chain.

---

## Design Principles

- **Crypto-native**: wallet address is the primary identifier.
- **Hardware-agnostic**: any NFC-capable microcontroller can be wired in.
- **Extensible**: Solana logic is centralized in the backend, so you can swap verification logic.
- **Transparent**: we prefer simple, auditable logic over opaque black-box firmware.
