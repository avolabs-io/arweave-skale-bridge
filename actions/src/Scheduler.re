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

type apolloQueryResult('a) =
  ApolloClient__ApolloClient.ApolloQueryResult.t('a);

//// QUERIES /////
module GetNextScheduledSyncs = [%graphql
  {|
query GetItemsReadyForSync ($endTime: Int!) {
  bridge_data(where: {next_scheduled_sync: { _lt: $endTime}, active: {_eq: true}}) {
    arweave_endpoint_rel {
      id
      user_id
      protocol
      url
      port
    }
    arweave_endpoint_id
    contentType
    frequency_duration_seconds
    id
    next_scheduled_sync
    skale_endpoint {
      uri
      user_id
    }
    skale_endpoint_id
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

//// MUTATIONS /////
module AddSyncItem = [%graphql
  {|
  mutation InsertBridgeSyncItem($bridgeId: Int!, $index: Int!, $startTime: Int!) {
    insert_bridge_sync_one(object: {Info: "", bridge_id: $bridgeId, index: $index, start_time: $startTime, status: "Initiated"}) {
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
  update_bridge_data_by_pk(pk_columns: {id: $id}, _inc: {next_scheduled_sync: $nextScheduledSync}) {
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
          Error(`Prometo_error(error->Obj.magic));
        | _ =>
          onError();
          Error(`Prometo_error("unknownError"->Obj.magic));
        }
      | Error(a) =>
        onError();
        Error(a);
      }
    });
};
type addSyncItemResult = {syncItemId: int};
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
  let endTime = currentTimestamp + updateInterval / 2;

  Client.instance
  ->ApolloClient.query(
      ~query=(module GetNextScheduledSyncs),
      {endTime: endTime},
    )
  ->mapAsync((result: apolloQueryResult(_)) =>
      GetNextScheduledSyncs.(
        switch (result) {
        | {data: Some({bridge_data})} =>
          bridge_data
          ->Array.map(
              (
                {
                  id,
                  frequency_duration_seconds,
                  bridge_sync_rel_aggregate: {aggregate: maxValue},
                  contentType,
                  skale_endpoint,
                },
              ) => {
              // TODO: this is temporary code. Will make update hasura so that the scale endpoint isn't an `optional`
              let skaleEndpointUri =
                skale_endpoint->Option.mapWithDefault(
                  "THIS SHOULD NEVER BE NULL - UPDATE HASURA ASAP", endpoint =>
                  endpoint.uri
                );

              let nextSyncTime = currentTimestamp + frequency_duration_seconds;

              updateSyncTime(
                ~onError=_ => Js.log("Unable to Sync Error."),
                id,
                nextSyncTime,
              )
              ->Prometo.flatMap(~f=_ => {
                  addSyncItem(
                    ~onError=_ => Js.log("error"),
                    ~startTime=currentTimestamp,
                    ~index=maxValue->getMaxIndexSyncFromAgregate + 1,
                    ~bridgeId=id,
                  )
                })
              ->Prometo.flatMap(~f=result => {
                  Skale.processDataFetching(
                    ~onError=_ => Js.log("Error fetching skale data"),
                    ~typeOfDataFetch=contentType,
                    ~endpoint=skaleEndpointUri,
                  )
                })
              ->Prometo.flatMap(~f=result => {
                  Upload.uploadChunkToArweave(
                    ~protocol="http",
                    ~port=4646,
                    ~host="Arweave test port",
                    ~onError=_ =>
                    Js.log("error uploading to arweave")
                  )
                })
              ->ignore;
            })
          ->ignore
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
