open Globals;
open BridgeSyncTypes;

[@bs.module "./skale-fetch-functions/blockData.js"]
external handleBlockData:
  (. string, int, string, string => unit, exn => unit) => unit =
  "default";

[@bs.module "./skale-fetch-functions/eventData.js"]
external handleEventData:
  (. string, int, string, string => unit, exn => unit) => unit =
  "default";
[@bs.module "./skale-fetch-functions/transactionData.js"]
external handleTransactionData:
  (. string, int, string, string => unit, exn => unit) => unit =
  "default";
[@bs.module "./skale-fetch-functions/transactionReceipts.js"]
external handleTransactionReceipts:
  (. string, int, string, string => unit, exn => unit) => unit =
  "default";
[@bs.module "./skale-fetch-functions/storageData.js"]
external handleStorageData:
  (. string, int, string, string => unit, exn => unit) => unit =
  "default";

exception InvalidOption(string);

let processDataFetching =
    (
      ~typeOfDataFetch,
      ~endpoint,
      ~chainId,
      ~onError,
      {syncItemId}: addSyncItemResult,
    ) => {
  Js.Promise.make((~resolve, ~reject) => {
    ignore(
      switch (typeOfDataFetch) {
      | "Block Data" =>
        let fileName = "block-data." ++ syncItemId->string_of_int;
        handleBlockData(.
          endpoint,
          chainId,
          fileName,
          path => resolve(. {syncItemId, path}),
          fetchException => reject(. fetchException),
        );
      | "Transactions Data" =>
        let fileName = "transaction-data." ++ syncItemId->string_of_int;
        handleTransactionData(.
          endpoint,
          chainId,
          fileName,
          path => resolve(. {syncItemId, path}),
          fetchException => reject(. fetchException),
        );
      | "Transaction Receipts" =>
        let fileName = "transaction-receipts." ++ syncItemId->string_of_int;
        handleTransactionReceipts(.
          endpoint,
          chainId,
          fileName,
          path => resolve(. {syncItemId, path}),
          fetchException => reject(. fetchException),
        );
      | "Event Data" =>
        let fileName = "event-data." ++ syncItemId->string_of_int;
        handleBlockData(.
          endpoint,
          chainId,
          fileName,
          path => resolve(. {syncItemId, path}),
          fetchException => reject(. fetchException),
        );
      | "Decentralized Storage" =>
        let fileName = "storage-data." ++ syncItemId->string_of_int;
        handleStorageData(.
          endpoint,
          chainId,
          fileName,
          path => resolve(. {syncItemId, path}),
          fetchException => reject(. fetchException),
        );
      | unrecognised => reject(. InvalidOption(unrecognised))
      },
    )
  })
  ->Prometo.fromPromise
  ->Prometo.handle(~f=result =>
      switch (result) {
      | Ok(dataFetchResult) => Ok(dataFetchResult)
      | Error(error) =>
        onError(error);
        Error(error);
      }
    );
};
