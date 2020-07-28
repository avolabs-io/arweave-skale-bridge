type addSyncItemResult = {syncItemId: int};
type skaleFetchResult = {
  syncItemId: int,
  path: string,
};

type arweaveUploadResult = {
  syncItemId: int,
  path: string,
  arweaveTransactionResult: Arweave.transactionResult,
};
