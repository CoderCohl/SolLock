# Backend API & Solana Integration

The backend is a **Node + TypeScript** server using Express.

## Endpoints

### `POST /auth`

Authorize a given wallet for a specific lock.

**Headers:**

- `x-lock-api-key: <ADMIN_API_KEY>`

**Body:**

```json
{
  "walletAddress": "4N9KcD8oQU...",
  "lockId": "front-door-1"
}
```

**Response:**

```json
{
  "ok": true,
  "walletAddress": "4N9KcD8oQU...",
  "lockId": "front-door-1"
}
```

### Policy Hook

Authorization is delegated to:

```ts
async function isWalletAuthorized(walletAddress: string, lockId: string): Promise<boolean>
```

The default implementation:

- Checks if wallet is in a local allowlist map.
- If `REQUIRED_NFT_MINT` is set:
  - Verifies that `walletAddress` holds at least 1 token of that mint using Solana RPC.

You can swap this logic to plug in:

- DAO membership
- custom SPL tokens
- multi-NFT rules
- subscription-like expiration
