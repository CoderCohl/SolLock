# Security Model & Limitations

SolLock is experimental and **not a replacement** for professional-grade access systems.

## Trust Assumptions

- The **backend** is trusted to make correct decisions.
- The **WiFi network** is reasonably secure.
- The **ESP32 firmware** is not physically tampered with.
- NFC tags are not trivially cloneable for your chosen use case.

## Attack Surfaces

- **NFC tag cloning**  
  Solution: Move to signature-based schemes where the wallet signs a nonce.

- **Backend compromise**  
  Solution: Harden the server, restrict to internal network, use HTTPS, monitor logs.

- **Firmware extraction & modification**  
  Solution: Use secure boot & flash encryption on compatible MCUs.

- **Replay attacks**  
  Solution: Introduce nonces and one-time tokens instead of static wallet address reads.

---

## Future Hardening Ideas

- Challenge–response protocol using Solana signatures.
- Encrypted communication channel between lock and backend.
- Per-session nonces stored temporarily on-chain.
- Hardware secure element for key material on the ESP32 side.

Use SolLock as a **research and prototype platform**, not as your only line of defense.
