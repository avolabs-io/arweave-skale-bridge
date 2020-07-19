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
let uploadChunkToArweave = () => {
  Js.log(
    "uploading to arweave endpoint",
    // based on result of arweave update, call SyncItemUpdate
  );
};
