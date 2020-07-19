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

// module TESTING = [%graphql
//   {|
// query MyQuery {
//   bridge_data {
//     active
//     arweave_endpoint_id
//     contentType
//     id
//     label
//     metaData
//     next_scheduled_sync
//     skale_endpoint_id
//     userId
//     frequency_setting {
//       frequency
//     }
//     bridge_sync_rel_aggregate(where: {}) {
//       aggregate {
//         max {
//           index
//         }
//       }
//     }
//   }
// }

// |}
// ];
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
    frequency_setting {
      frequency
    }
    id
    next_scheduled_sync
    skale_endpoint {
      uri
      user_id
    }
    skale_endpoint_id
    userId
    bridge_sync_rel_aggregate(where: {}) {
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
/*
 bridge_sync_rel_aggregate(where: {}) {
   aggregate {
     max {
       index
     }
   }
 } */

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
mutation ScheduleNextSyncTime($id: Int!, $frequencyDurationSeconds: Int!) {
  update_bridge_data_by_pk(pk_columns: {id: $id}, _inc: {next_scheduled_sync: $frequencyDurationSeconds}) {
    id
  }
}
|}
];

let addSyncItem = (~bridgeId, ~index, ~startTime) => {
  Client.instance
  ->ApolloClient.mutate(
      ~mutation=(module AddSyncItem),
      {bridgeId, index, startTime},
    )
  ->ignore;
  // TODO: id of bridgeSyncItem returned, so can be passed on to uploadChunkToArweave function
};

let updateSyncTime = (id, frequencyDurationSeconds) => {
  Client.instance
  ->ApolloClient.mutate(
      ~mutation=(module UpdateNextSync),
      {id, frequencyDurationSeconds},
    )
  ->ignore;
};

// for every item.
// create a sync Item In the table.
// mutation to update when next sync should be (based on frequency). (does this change if current sync fails.)
// call uploadChunkToArweave
let processBridges = updateInterval => {
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
          ->Array.map(({id, frequency_duration_seconds}) => {
              addSyncItem(
                ~bridgeId=id,
                ~index=1 + 1,
                ~startTime=currentTimestamp,
              );
              updateSyncTime(id, frequency_duration_seconds);
              Upload.uploadChunkToArweave();
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

let startScheduler = updateInterval =>
  Js.Global.setInterval(
    () => {
      Js.log("processing the bridges");
      processBridges(updateInterval);
    },
    updateInterval,
  );
