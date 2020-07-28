/**
 * Basic idea:
 * Chron runs every hour - on the hour.
 * It queries for bridges (from hasura) that should be executed based on the time their next scheduled.
 * Each bridge that is found is executed according to its settings. It is recorded in Hasura when the execution started and updates to the status of that sync are also recorded.
 *
 * Edge cases:
 * When a new bridge is recorded it must be executed on the nearest hour to start the process.
 * If the user wants a bridge to execute at a specific time they can 'fake' the last time updated so that it is picked up by the process.
 */;

open BsCron;
open Globals;
open BridgeSyncTypes;

type apolloQueryResult('a) =
  ApolloClient__ApolloClient.ApolloQueryResult.t('a);

//// QUERIES /////
module GetNextScheduledSyncs = [%graphql
  {|
query GetItemsReadyForSync ($endTime: Int!) {
  bridge_data(where: {next_scheduled_sync: { _lt: $endTime}, active: {_eq: true}}) {
    arweave_endpoint_rel {
      protocol
      url
      port
    }
    contentType
    frequency_duration_seconds
    id
    next_scheduled_sync
    skale_endpoint {
      uri
      chain_id
    }
    userId
    bridge_sync_rel_aggregate {
      aggregate {
        max {
          index
        }
      }
    }
  }
}
|}
];

module GetArweaveWallet = [%graphql
  {|
query ArweaveWallet($userId: String!) {
  arweave_key(where: {user_id: {_eq: $userId}}) {
    pub_key
    priv_key
    user_id
  }
}
|}
];

//// MUTATIONS /////
module AddSyncItem = [%graphql
  {|
  mutation InsertBridgeSyncItem($bridgeId: Int!, $index: Int!, $startTime: Int!) {
    insert_bridge_sync_one(object: {info: "", bridge_id: $bridgeId, index: $index, start_time: $startTime, status: "Initiated"}) {
      id
    }
  }
|}
];
module CompleteSync = [%graphql
  {|
  mutation CompleteSync($id: Int!, $endTime: Int!, $arweaveTxId: String!) {
    update_bridge_sync_by_pk(pk_columns: {id: $id}, _set: {end_time: $endTime, status: "complete", arweave_tx_id: $arweaveTxId}) {
      id
    }
  }
|}
];
module ReportStatus = [%graphql
  {|
  mutation ReportError($id: Int!, $status: String!) {
    update_bridge_sync_by_pk(pk_columns: {id: $id}, _set: {status: $status}) {
      id
    }
  }
|}
];
module ReportError = [%graphql
  {|
  mutation ReportError($id: Int!, $errorMessage: String!, $errorStackTrace: String!) {
    update_bridge_sync_by_pk(pk_columns: {id: $id}, _set: {error_message: $errorMessage, error_stack_trace: $errorStackTrace}) {
      id
    }
  }
|}
];

// Instead of an increment maybe we want this as a set.
// Then if an arweave upload fails, we could set the sync time back to now to try again "immediately"
module UpdateNextSync = [%graphql
  {|
mutation ScheduleNextSyncTime($id: Int!, $nextScheduledSync: Int!) {
  update_bridge_data_by_pk(pk_columns: {id: $id}, _set: {next_scheduled_sync: $nextScheduledSync}) {
    id
  }
}
|}
];

let handleMutateErrorPromise = (promise, ~onError) => {
  promise
  ->Prometo.fromPromise
  ->Prometo.handle(
      ~f=(
           result:
             Pervasives.result(
               ApolloClient__ApolloClient.FetchResult.t(_),
               [> Prometo.error],
             ),
         ) => {
      switch (result) {
      | Ok(mutationResult) =>
        switch (mutationResult) {
        | {data: Some(data), errors: None} => Ok(data)
        | {errors: Some(error)} =>
          onError();
          Js.log(error);
          Error(`Prometo_error(error->Obj.magic));
        | _ =>
          onError();
          Js.log("error: unknown");
          Error(`Prometo_error("unknownError"->Obj.magic));
        }
      | Error(a) =>
        onError();
        Js.log2("error: ", a);
        Error(a);
      }
    });
};
let addSyncItem = (~onError, ~bridgeId, ~index, ~startTime) => {
  Client.instance
  ->ApolloClient.mutate(
      ~mutation=(module AddSyncItem),
      {bridgeId, index, startTime},
    )
  ->handleMutateErrorPromise(~onError)
  ->Prometo.map(~f=(input: AddSyncItem.t) => {
      let itemId =
        input.insert_bridge_sync_one
        ->Option.mapWithDefault(
            999 /*TODO: investigate why this is optional.*/, sync =>
            sync.id
          );
      {syncItemId: itemId};
    });
};

let updateSyncTime = (~onError, id, nextScheduledSync) => {
  Client.instance
  ->ApolloClient.mutate(
      ~mutation=(module UpdateNextSync),
      {id, nextScheduledSync},
    )
  ->handleMutateErrorPromise(~onError);
};

let reportError = (~id, ~errorMessage, ~errorStackTrace) => {
  Js.log2("id of bridge that errored", id);
  Client.instance
  ->ApolloClient.mutate(
      ~mutation=(module ReportError),
      ReportError.makeVariables(~id, ~errorMessage, ~errorStackTrace, ()),
    )
  ->ignore;
};

let completeSync = (~onError, ~id, ~arweaveTxId) => {
  let currentTimestamp = Js.Math.floor_int(Js.Date.now() /. 1000.);
  Client.instance
  ->ApolloClient.mutate(
      ~mutation=(module CompleteSync),
      CompleteSync.makeVariables(
        ~id,
        ~endTime=currentTimestamp,
        ~arweaveTxId,
        (),
      ),
    )
  ->handleMutateErrorPromise(~onError);
};

let getMaxIndexSyncFromAgregate =
    (
      maxAgregate:
        option(
          GetNextScheduledSyncs.t_bridge_data_bridge_sync_rel_aggregate_aggregate,
        ),
    ) => {
  maxAgregate
  ->Option.flatMap(obj => {obj.max})
  ->Option.flatMap(max => {max.index})
  ->Option.getWithDefault(0);
};

// for every item.
// create a sync Item In the table.
// mutation to update when next sync should be (based on frequency). (does this change if current sync fails.)
// call uploadChunkToArweave
let processBridges = updateInterval => {
  Js.log("processing the bridges");

  let currentTimestamp = Js.Math.floor_int(Js.Date.now() /. 1000.);
  // NOTE: Here we take the end time to be halfway between this interval and the next.
  //       This means that it processes any future items that are closer to this interval than the next interval.
  //       The idea is that this will prevent 'drift', where you'll have an off-by-one interval.
  let endTime = currentTimestamp + updateInterval / 1000 / 2;

  Client.instance
  ->ApolloClient.query(
      ~query=(module GetNextScheduledSyncs),
      {endTime: endTime},
    )
  ->mapAsync((result: apolloQueryResult(_)) =>
      GetNextScheduledSyncs.(
        switch (result) {
        | {data: Some({bridge_data})} =>
          Js.log("hello");

          bridge_data
          ->Array.map(
              (
                {
                  id,
                  frequency_duration_seconds,
                  bridge_sync_rel_aggregate: {aggregate: maxValue},
                  contentType,
                  skale_endpoint,
                  userId,
                  arweave_endpoint_rel,
                },
              ) => {
              // TODO: this is temporary code. Will make update hasura so that the scale endpoint isn't an `optional`
              let {uri: skaleEndpointUri, chain_id} =
                skale_endpoint->Option.mapWithDefault(
                  {
                    uri: "THIS SHOULD NEVER BE NULL - UPDATE HASURA ASAP",
                    chain_id: 0,
                  },
                  endpoint =>
                  endpoint
                );
              let {protocol, url: host, port} =
                arweave_endpoint_rel->Option.mapWithDefault(
                  {
                    url: "THIS SHOULD NEVER BE NULL - UPDATE HASURA ASAP",
                    protocol: "",
                    port: 0,
                  },
                  endpoint =>
                  endpoint
                );

              let nextSyncTime = currentTimestamp + frequency_duration_seconds;

              Client.instance
              ->ApolloClient.query(
                  ~query=(module GetArweaveWallet),
                  GetArweaveWallet.makeVariables(~userId, ()),
                )
              ->mapAsync((result: apolloQueryResult(_)) =>
                  GetArweaveWallet.(
                    switch (result) {
                    | {data: Some({arweave_key})} =>
                      // let ({pub_key, priv_key}) = arweave_key[]
                      let arweaveKey = arweave_key[0];
                      switch (arweaveKey) {
                      | Some({pub_key, priv_key}) =>
                        // TODO: use the correct decoder to prevent malformed private keys from being accepted.
                        let privKeyResult = priv_key->Arweave.jwk_decode;

                        switch (privKeyResult) {
                        | Ok(privKey) =>
                          updateSyncTime(
                            ~onError=_ => Js.log("Unable to Sync Error."),
                            id,
                            nextSyncTime,
                          )
                          ->Prometo.flatMap(~f=_ => {
                              addSyncItem(
                                ~onError=
                                  () => {
                                    Js.log(
                                      "ERROR: couldn't record bridge sync.",
                                    )
                                  },
                                ~startTime=currentTimestamp,
                                ~index=
                                  maxValue->getMaxIndexSyncFromAgregate + 1,
                                ~bridgeId=id,
                              )
                            })
                          ->Prometo.flatMap(~f=result => {
                              Skale.processDataFetching(
                                ~onError=reportError(~id=result.syncItemId),
                                ~typeOfDataFetch=contentType,
                                ~endpoint=skaleEndpointUri,
                                ~chainId=chain_id,
                                result,
                              )
                            })
                          ->Prometo.flatMap(~f=result => {
                              Upload.uploadChunkToArweave(
                                ~protocol,
                                ~port,
                                ~host,
                                ~privKey,
                                ~onError=reportError(~id=result.syncItemId),
                                result,
                              )
                            })
                          ->Prometo.flatMap(~f=result => {
                              completeSync(
                                ~onError=
                                  _ =>
                                    Js.log(
                                      "there was an error marking this sync as complete",
                                    ),
                                ~id=result.syncItemId,
                                ~arweaveTxId=
                                  result.arweaveTransactionResult.id,
                              )
                            })
                          ->ignore
                        | Error(error) => Js.log(error)
                        };
                      | None =>
                        Js.log("ERROR: unable to load users arweave keys!")
                      };
                      ();
                    | _ =>
                      Js.log("Error! User doesn't have any arweave keys!")
                    }
                  )
                )
              ->ignore;
            })
          ->ignore;
          Js.log("End");
        | _ => Js.Exn.raiseError("Error: failed to query")
        }
      )
    )
  ->catchAsync(error => Js.Promise.resolve(Js.log2("Error: ", error)))
  ->ignore;
};

//// NOTE: only kept for reference if we ever plan to move back to using cron jobs.
// let job =
//   CronJob.make(
//     `CronString("* * * * * *"), // every second
//     // `CronString("* * * * *"), // every minute
//     //`CronString("* * * *"), // every hour
//     _ => {Js.log("every hour")},
//     (),
//   );

let startScheduler = updateInterval => {
  processBridges(updateInterval);

  Js.Global.setInterval(
    () => {processBridges(updateInterval)},
    updateInterval,
  );
};
