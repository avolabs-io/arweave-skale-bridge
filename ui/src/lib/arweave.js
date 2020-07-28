// https://github.com/ArweaveTeam/arweave-js
const Arweave = require("arweave/node");

const arweaveMainetConfig = {
  host: "arweave.net", // Hostname or IP address for a Arweave host
  port: 443, // Port
  protocol: "https", // Network protocol http or https
  timeout: 10 * 60 * 1000, // Network request timeouts in milliseconds
  logging: false,
};

const arweave = Arweave.init(arweaveMainetConfig);

const convertWinstonToArweave = (winstonAmount) => {
  let arweaveAmount = arweave.ar.winstonToAr(winstonAmount);
  return arweaveAmount;
};

const getWalletBalanceInWinston = async (publicKey) => {
  const balanceInWinston = await arweave.wallets.getBalance(publicKey);
  return balanceInWinston;
};

const getWalletBalanceArweave = async (publicKey) => {
  let result = await getWalletBalanceInWinston(publicKey);
  let arweaveAmount = convertWinstonToArweave(result);
  return arweaveAmount;
};

export default {
  arweave,
  getWalletBalanceInWinston,
  convertWinstonToArweave,
  getWalletBalanceArweave,
};
