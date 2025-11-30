import React, { useState } from "react";

const gradientText = {
  background: "linear-gradient(90deg,#9945FF,#14F195)",
  WebkitBackgroundClip: "text",
  WebkitTextFillColor: "transparent"
};

function App() {
  const [wallet, setWallet] = useState("");
  const [lockId, setLockId] = useState("front-door-1");
  const [result, setResult] = useState(null);

  async function simulateAuth() {
    setResult("...");
    const res = await fetch("http://localhost:3000/auth", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "x-lock-api-key": "super-secret-lock-key"
      },
      body: JSON.stringify({ walletAddress: wallet, lockId })
    });
    const data = await res.json();
    setResult(data);
  }

  return (
    <div style={{
      minHeight: "100vh",
      background: "radial-gradient(circle at top,#0a0b1c,#020308)",
      color: "#fff",
      display: "flex",
      alignItems: "center",
      justifyContent: "center",
      fontFamily: "system-ui, -apple-system, BlinkMacSystemFont"
    }}>
      <div style={{
        width: 480,
        padding: 32,
        borderRadius: 24,
        background: "rgba(8,12,32,0.9)",
        boxShadow: "0 0 60px rgba(153,69,255,0.4)"
      }}>
        <h1 style={{ ...gradientText, fontSize: 32, fontWeight: 800, textAlign: "center" }}>
          SolLock Dashboard
        </h1>
        <p style={{ opacity: 0.8, fontSize: 14, textAlign: "center", marginBottom: 24 }}>
          Simulate NFC lock authorization using a wallet address and lock ID.
        </p>

        <label style={{ fontSize: 12, opacity: 0.9 }}>Wallet Address</label>
        <input
          value={wallet}
          onChange={e => setWallet(e.target.value)}
          placeholder="WALLET_04AABBCCDD"
          style={{
            width: "100%",
            marginTop: 4,
            marginBottom: 16,
            padding: "10px 12px",
            borderRadius: 12,
            border: "1px solid rgba(148,163,184,0.4)",
            background: "rgba(15,23,42,0.9)",
            color: "#e5e7eb"
          }}
        />

        <label style={{ fontSize: 12, opacity: 0.9 }}>Lock ID</label>
        <input
          value={lockId}
          onChange={e => setLockId(e.target.value)}
          style={{
            width: "100%",
            marginTop: 4,
            marginBottom: 24,
            padding: "10px 12px",
            borderRadius: 12,
            border: "1px solid rgba(148,163,184,0.4)",
            background: "rgba(15,23,42,0.9)",
            color: "#e5e7eb"
          }}
        />

        <button
          onClick={simulateAuth}
          style={{
            width: "100%",
            padding: "10px 0",
            borderRadius: 999,
            border: "none",
            fontWeight: 700,
            fontSize: 14,
            backgroundImage: "linear-gradient(90deg,#9945FF,#14F195)",
            cursor: "pointer"
          }}
        >
          Test Authorization
        </button>

        <div style={{ marginTop: 24, fontSize: 13 }}>
          <div style={{ opacity: 0.7, marginBottom: 4 }}>Result:</div>
          <pre style={{
            background: "rgba(15,23,42,0.9)",
            borderRadius: 12,
            padding: 12,
            border: "1px solid rgba(55,65,81,0.6)",
            overflowX: "auto"
          }}>
            {result === null ? "No request yet." : JSON.stringify(result, null, 2)}
          </pre>
        </div>
      </div>
    </div>
  );
}

export default App;
