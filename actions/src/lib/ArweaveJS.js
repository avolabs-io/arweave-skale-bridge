// https://github.com/ArweaveTeam/arweave-js
const fs = require("fs");
const path = require("path");

const Arweave = require("arweave/node");

// const arweaveMainetConfig = {
//   host: "arweave.net", // Hostname or IP address for a Arweave host
//   port: 443, // Port
//   protocol: "https", // Network protocol http or https
//   timeout: 10 * 60 * 1000, // Network request timeouts in milliseconds
//   logging: false,
// };

// const arweave = Arweave.init(arweaveMainetConfig);

const connectWallet = async (pvtKey, arweave) => {
  return await arweave.wallets.jwkToAddress(pvtKey);
};

const createDataTransaction = async (content, arweave) => {
  console.log("store data on arweave");
  let key = await arweave.wallets.generate();

  let transaction = await arweave.createTransaction(
    {
      data: content,
    },
    key
  );

  return transaction;
};

const addTagsToTransaction = (transaction, tags) => {
  // transaction.addTag("Content-Type", "application/x-gzip");
  transaction.addTag("key2", "value2");
};

const getTransaction = (transactionId, arweave) =>
  arweave.transactions.get(transactionId).then((transaction) => {
    console.log(transaction);
  });

const sleep = (ms) => new Promise((res) => setTimeout(res, ms));

const uploadDataToArweave = async (pvtKey, arweave, pathToData) => {
  await connectWallet(pvtKey, arweave);
  // const filePath = path.join(__dirname, pathToData);
  console.log("filePath");
  console.log(pathToData);
  const file = await new Promise((resolve, reject) =>
    fs.readFile(pathToData, { encoding: "utf-8" }, function (err, data) {
      if (!err) {
        console.log("received data ");
        resolve(data);
      } else {
        console.log(err);
        reject(err);
      }
    })
  );
  const transaction = await createDataTransaction(file, arweave);
  transaction.addTag("skale-arweave-bridge", "testing");
  transaction.addTag("path", pathToData);
  console.log(transaction);

  await arweave.transactions.sign(transaction, pvtKey);

  console.log(transaction);

  let uploader = await arweave.transactions.getUploader(transaction);

  while (!uploader.isComplete) {
    await uploader.uploadChunk();
    console.log(
      `${uploader.pctComplete}% complete, ${uploader.uploadedChunks}/${uploader.totalChunks}`
    );
  }

  // const publicKey = await arweave.wallets.jwkToAddress(pvtKey);
  // console.log("publicKey:  ", publicKey);
  // const prevTransactionId = await new Promise(async (res, rej) => {
  //   let hasFoundId = false;
  //   while (!hasFoundId) {
  //     const prevTransactionId = await getWalletLastTxId(publicKey, arweave);
  //     console.log("waiting for tx id", prevTransactionId);

  //     if (!!prevTransactionId) {
  //       hasFoundId = true;
  //       res(prevTransactionId);
  //     } else {
  //       await sleep(1000);
  //     }
  //   }
  // });
  // console.log("prevTransactionId:  ", prevTransactionId);

  // const transactionData = await getTransactionData(prevTransactionId, arweave);

  // console.log("transactionData:  ", transactionData);

  // const data = await getTransactionData(
  //   "eyyW-Q0k2VN7ukP47nKc6LqpBod9uneKWcer8xODDt4",
  //   arweave
  // );
  // console.log("data: ", data);

  return transaction;
};

const getTransactionData = async (transactionId, arweave) => {
  const data = await arweave.transactions.getData(transactionId, {
    decode: true,
    string: true,
  });
  return data;
};

const getWalletLastTxId = async (publicKey, arweave) => {
  arweave.wallets.getLastTransactionID(publicKey).then((transactionId) => {
    console.log(transactionId);
  });
};

module.exports = {
  connectWallet,
  createDataTransaction,
  addTagsToTransaction,
  getTransaction,
  uploadDataToArweave,
  getTransactionData,
  getWalletLastTxId,
};
