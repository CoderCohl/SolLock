import "dotenv/config";
import express from "express";
import morgan from "morgan";
import cors from "cors";
import { walletOwnsRequiredNFT } from "./solana";

const app = express();
const port = process.env.PORT || 3000;
const ADMIN_API_KEY = process.env.ADMIN_API_KEY || "super-secret-lock-key";

app.use(cors());
app.use(express.json());
app.use(morgan("dev"));

type AllowlistMap = {
  [lockId: string]: string[];
};

const allowlist: AllowlistMap = {
  "front-door-1": [
    "WALLET_04AABBCCDD",
    "WALLET_1122334455"
  ]
};

function authLock(req: express.Request, res: express.Response, next: express.NextFunction) {
  const apiKey = req.header("x-lock-api-key");
  if (!apiKey || apiKey !== ADMIN_API_KEY) {
    return res.status(401).json({ ok: false, error: "unauthorized lock" });
  }
  next();
}

async function isWalletAuthorized(walletAddress: string, lockId: string): Promise<boolean> {
  const list = allowlist[lockId] || [];
  const inAllowlist = list.includes(walletAddress);

  const ownsNFT = await walletOwnsRequiredNFT(walletAddress);

  if (!inAllowlist) return false;
  if (!ownsNFT) return false;
  return true;
}

app.get("/", (_req, res) => {
  res.send("SolLock backend online");
});

app.post("/auth", authLock, async (req, res) => {
  const { walletAddress, lockId } = (req.body || {}) as { walletAddress?: string; lockId?: string };
  if (!walletAddress || typeof walletAddress !== "string") {
    return res.status(400).json({ ok: false, error: "walletAddress required" });
  }

  const lid = typeof lockId === "string" ? lockId : "default-lock";

  try {
    const ok = await isWalletAuthorized(walletAddress, lid);
    return res.json({ ok, walletAddress, lockId: lid });
  } catch (err) {
    console.error("Auth error", err);
    return res.status(500).json({ ok: false, error: "internal error" });
  }
});

app.listen(port, () => {
  console.log(`SolLock backend listening on port ${port}`);
});
