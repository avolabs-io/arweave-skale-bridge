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

module GetNextScheduledSyncs = [%graphql
  {|
query MyQuery ($startTime: Int!, $endTime: Int!) {
  bridge_data(where: {next_scheduled_sync: {_gt: $startTime, _lt: $endTime}, active: {_eq: true}}) {
    active
    arweave_endpoint {
      id
      uri
      user_id
    }
    arweave_endpoint_id
    contentType
    frequency
    frequency_setting {
      frequency
    }
    id
    label
    metaData
    next_scheduled_sync
    skale_endpoint {
      uri
      user_id
    }
    skale_endpoint_id
    userId
  }
}
|}
];

let processBridges = () => {
  let currentTimestamp = Js.Math.floor_int(Js.Date.now() /. 1000.);
  let startTime = currentTimestamp - 1800;
  let endTime = currentTimestamp + 1800;
  Client.instance
  ->ApolloClient.query(
      ~query=(module GetNextScheduledSyncs),
      {startTime, endTime},
    )
  ->ignore;

  ();
};

let job =
  CronJob.make(
    `CronString("* * * * * *"), // every second
    // `CronString("* * * * *"), // every minute
    //`CronString("* * * *"), // every hour
    _ => {Js.log("every hour")},
    (),
  );

let startScheduler = () => start(job);
