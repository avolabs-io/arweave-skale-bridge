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

const waitForTxStatus = async (arweave, transactionId) => {
  return await new Promise((res, rej) =>
    arweave.transactions.getStatus(transactionId).then((status) => res(status))
  );
};

const waitForTxStatusSucceedFail = async (
  arweave,
  transactionId,
  pushStatus
) => {
  while (true) {
    pushStatus(`Submitted but waiting for confirmation from arweave`);
    const status = await waitForTxStatus(arweave, transactionId);
    if (status.status === 200) {
      return status;
    } else if (status.status === 202) {
      await sleep(2000);
    } else {
      // If it isn't 202 or 200 it must be some kind of error. (better control over specific codes can be added)
      pushStatus(`Arweave rejected the transaction`);
      return status;
    }
  }
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

  // Add a very small sleep here to prevent out of order data setting.
  await sleep(100);

  const status = await waitForTxStatusSucceedFail(
    arweave,
    transaction.id,
    pushStatus
  );
  if (status.status != 200) {
    throw Error(
      "Arweave (id:",
      transaction.id,
      "rejected transaction with status: " + JSON.stringify(status)
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
