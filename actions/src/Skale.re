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
      ~pushStatus,
      {syncItemId}: addSyncItemResult,
    ) => {
  Js.Promise.make((~resolve, ~reject) => {
    pushStatus("Fetching Skale Data");

    let resolveAndSetState = path => {
      pushStatus("Data retrieved from Skale");
      resolve(. {syncItemId, path});
    };

    ignore(
      switch (typeOfDataFetch) {
      | "Block Data" =>
        let fileName = "block-data." ++ syncItemId->string_of_int;
        handleBlockData(.
           endpoint, chainId, fileName, resolveAndSetState, fetchException =>
          reject(. fetchException)
        );
      | "Transactions Data" =>
        let fileName = "transaction-data." ++ syncItemId->string_of_int;
        handleTransactionData(.
           endpoint, chainId, fileName, resolveAndSetState, fetchException =>
          reject(. fetchException)
        );
      | "Transaction Receipts" =>
        let fileName = "transaction-receipts." ++ syncItemId->string_of_int;
        handleTransactionReceipts(.
           endpoint, chainId, fileName, resolveAndSetState, fetchException =>
          reject(. fetchException)
        );
      | "Event Data" =>
        let fileName = "event-data." ++ syncItemId->string_of_int;
        handleBlockData(.
           endpoint, chainId, fileName, resolveAndSetState, fetchException =>
          reject(. fetchException)
        );
      | "Decentralized Storage" =>
        let fileName = "storage-data." ++ syncItemId->string_of_int;
        handleStorageData(.
           endpoint, chainId, fileName, resolveAndSetState, fetchException =>
          reject(. fetchException)
        );
      | unrecognised => reject(. InvalidOption(unrecognised))
      },
    );
  })
  ->Prometo.fromPromise
  ->Prometo.handle(~f=result =>
      switch (result) {
      | Ok(dataFetchResult) => Ok(dataFetchResult)
      | Error(error) =>
        switch (error) {
        | `Prometo_error(jsError) =>
          let (errorMessage, errorStackTrace) =
            Util.errorToMessageAndStacktrace(jsError);
          Js.log3("IMPORTANT ERROR", error, errorStackTrace);
          onError(~errorMessage, ~errorStackTrace);
        | _ =>
          onError(
            ~errorMessage=
              "Unknown error when fetching Skale data. Please report to support.",
            ~errorStackTrace="No stack trace",
          );
          Js.log(
            "WARNING - unknown error when fetching skale data! Investigate this.",
          );
        };
        Error(error);
      }
    );
};
