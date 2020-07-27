open Globals;
open BridgeSyncTypes;

[@bs.module "./skale-fetch-functions/blockData.js"]
external handleBlockData:
  (. string, string, string => unit, exn => unit) => unit =
  "default";

exception InvalidOption(string);

let processDataFetching =
    (~typeOfDataFetch, ~endpoint, ~onError, {syncItemId}: addSyncItemResult) => {
  Js.Promise.make((~resolve, ~reject) => {
    ignore(
      switch (typeOfDataFetch) {
      | "Block Data"
      | "Transactions Data"
      | "Transaction Receipts"
      | "Event Data"
      | "Decentralized Storage" =>
        let fileName = "block-data." ++ syncItemId->string_of_int;
        handleBlockData(.
          endpoint,
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
