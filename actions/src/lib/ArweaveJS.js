// https://github.com/ArweaveTeam/arweave-js
const fs = require("fs");
const path = require("path");

const connectWallet = async (pvtKey, arweave) => {
  return await arweave.wallets.jwkToAddress(pvtKey);
};

const createDataTransaction = async (content, arweave, pvtKey) => {
  console.log("store data on arweave");

  let transaction = await arweave.createTransaction(
    {
      data: content,
    },
    pvtKey
  );

  return transaction;
};

const addTagsToTransaction = (transaction, tags) => {
  transaction.addTag("Content-Type", "application/x-gzip");
  transaction.addTag("key2", "value2");
};

const getTransaction = (transactionId, arweave) =>
  arweave.transactions.get(transactionId).then((transaction) => {
    console.log(transaction);
  });

const sleep = (ms) => new Promise((res) => setTimeout(res, ms));

const getTransactionData = async (transactionId, arweave) => {
  await arweave.transactions
    .getData(transactionId, {
      decode: true,
      string: true,
    })
    .then((data) => {
      console.log("data: ", data);
    });
};

const uploadDataToArweave = async (key, arweave, pathToData, pushStatus) => {
  pushStatus("Starting arweave upload");
  let data = fs.readFileSync(pathToData);

  let transaction = await arweave.createTransaction({ data: data }, key);

  // TODO: add tags!

  await arweave.transactions.sign(transaction, key);

  let uploader = await arweave.transactions.getUploader(transaction);

  while (!uploader.isComplete) {
    await uploader.uploadChunk();
    pushStatus(
      `Upload: ${uploader.pctComplete}% complete, ${uploader.uploadedChunks}/${uploader.totalChunks}`
    );
  }

  return {
    format: transaction.format,
    id: transaction.id,
    last_tx: transaction.last_tx,
    owner: transaction.owner,
    tags: transaction.tags,
    target: transaction.target,
    quantity: transaction.quantity,
    data_size: transaction.data_size,
    data_root: transaction.data_root,
    reward: transaction.reward,
    signature: transaction.signature,
  };
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
