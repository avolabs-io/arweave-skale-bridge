open Globals;
open BridgeSyncTypes;

[@bs.module "./lib/ArweaveJS.js"]
external uploadDataToArweave:
  (. Arweave.jwk, Arweave.t, string, string => unit) =>
  Js.Promise.t(Arweave.transactionResult) =
  "uploadDataToArweave";

// Mutation to change sync status + update endTime.
module SyncItemUpdate = [%graphql
  {|
    mutation SyncItemMutation($id: Int!, $endTime: Int!, $status: String!) {
    update_bridge_sync_by_pk(pk_columns: {id: $id}, _set: {end_time: $endTime, status: $status}) {
        end_time
        status
    }
  }
|}
];

exception FileNotFound(string);

// Will take in the skaleEndPoint (so we know what data to upload)
// The arweave endpoint so we know where to upload it
// What else? Should use users arweave keys for this action
let uploadChunkToArweave =
    (
      ~protocol,
      ~port,
      ~host,
      ~onError,
      ~privKey,
      ~pushStatus,
      {syncItemId, path}: BridgeSyncTypes.skaleFetchResult,
    ) => {
  Js.log(
    "data at " ++ path ++ " to arweave endpoint",
    // based on result of arweave update, call SyncItemUpdate
  );
  // MOCKED FUNCTION.
  Js.log4("Uploading arweave data to", protocol, host, port);
  Js.Promise.make((~resolve, ~reject) => {
    ignore(
      Js.Global.setTimeout(
        () => {
          let arweaveInstance =
            Arweave.init({
              host,
              port,
              protocol,
              timeout: Some(10 * 60 * 1000),
              logging: Some(false),
            });

          NodeJs.(
            if (Fs.existsSync(path)) {
              let file = Fs.readFileSync(path, ());
              // upload to arweave function (filePath)
              uploadDataToArweave(.
                privKey,
                arweaveInstance,
                path,
                pushStatus,
              )
              ->Js.Promise.then_(
                  arweaveTransactionResult => {
                    Fs.unlinkSync(path);
                    resolve(. {syncItemId, path, arweaveTransactionResult})
                    ->async;
                  },
                  _,
                )
              // The catch below is a bit of a hack, but it works. This should be reworked soon!
              ->Js.Promise.catch(err => reject(. err->Obj.magic)->async, _)
              ->ignore;
            } else {
              reject(. FileNotFound("file not found at: " ++ path));
            }
          );
        },
        1000,
      ),
    )
  })
  ->Prometo.fromPromise
  ->Prometo.handle(~f=result =>
      switch (result) {
      | Ok(uploadResult) => Ok(uploadResult)
      | Error(error) =>
        switch (error) {
        | `Prometo_error(jsError) =>
          let (errorMessage, optErrorStackTrace) =
            Util.errorToMessageAndStacktrace(jsError);
          onError(
            ~errorMessage,
            ~errorStackTrace=
              optErrorStackTrace->Option.getWithDefault(
                "No stacktrace available",
              ),
          );
        | _ =>
          onError(
            ~errorMessage=
              "Unknown error when fetching uploading data to arweave. Please report to support.",
            ~errorStackTrace="No stack trace",
          );
          Js.log(
            "WARNING - unknown error when uploading data to arweave! Investigate this.",
          );
        };
        Error(error);
      }
    );
};
