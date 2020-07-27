open Globals;

// Mutation to change sync status + update endTime.
module SyncItemUpdate = [%graphql
  {|
    mutation SyncItemMutation($Id: Int!, $endTime: Int!, $status: String!) {
    update_bridge_sync_by_pk(pk_columns: {id: $Id}, _set: {end_time: $endTime, status: $status}) {
        end_time
        status
    }
  }
|}
];

// Will take in the skaleEndPoint (so we know what data to upload)
// The arweave endpoint so we know where to upload it
// What else? Should use users arweave keys for this action
let uploadChunkToArweave =
    (
      ~protocol,
      ~port,
      ~host,
      ~onError,
      previousStepResult: BridgeSyncTypes.addSyncItemResult,
    ) => {
  Js.log(
    "uploading to arweave endpoint",
    // based on result of arweave update, call SyncItemUpdate
  );
  // MOCKED FUNCTION.
  Js.log4("Uploading arweave data to", protocol, host, port);
  Js.Promise.make((~resolve, ~reject as _) => {
    ignore(Js.Global.setTimeout(() => resolve(. previousStepResult), 1000))
  })
  ->Prometo.fromPromise
  ->Prometo.handle(~f=result =>
      switch (result) {
      | Ok(uploadResult) => Ok(uploadResult)
      | Error(error) =>
        onError(error);
        Error(error);
      }
    );
};
