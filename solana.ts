import { Connection, PublicKey } from "@solana/web3.js";

const RPC_URL = process.env.SOLANA_RPC_URL || "https://api.mainnet-beta.solana.com";
const REQUIRED_NFT_MINT = process.env.REQUIRED_NFT_MINT; // optional

const connection = new Connection(RPC_URL, "confirmed");

export async function walletOwnsRequiredNFT(walletAddress: string): Promise<boolean> {
  if (!REQUIRED_NFT_MINT) return true; // NFT not required

  const owner = new PublicKey(walletAddress);
  const mint = new PublicKey(REQUIRED_NFT_MINT);

  const tokenAccounts = await connection.getTokenAccountsByOwner(owner, {
    mint
  });

  return tokenAccounts.value.length > 0;
}
