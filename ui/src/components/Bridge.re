open Globals;

module BridgeSyncsQuery = [%graphql
  {|
subscription Syncs($bridgeId: Int!) {
  bridge_sync(where: {bridge_id: {_eq: $bridgeId}}, limit: 30, order_by: [{index: desc}]) {
    id
    info
    end_time
    index
    start_time
    status
  }
}
|}
];

[@react.component]
let make = (~bridgeId) => {
  let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();

  let usersBridgeQueryResult =
    BridgeSyncsQuery.use(
      // ~fetchPolicy=CacheAndNetwork,
      // ~errorPolicy=All,
      BridgeSyncsQuery.makeVariables(~bridgeId, ()),
    );

  <div>
    {("Info about the bridge #" ++ bridgeId->string_of_int)->React.string}
    <table>
      {switch (usersBridgeQueryResult) {
       | {loading: true, data: None} => <p> "Loading"->React.string </p>
       | {loading, data: Some(data), error} =>
         <>
           <tr>
             <th> "id"->React.string </th>
             <th> "end_time"->React.string </th>
             <th> "Sync #"->React.string </th>
             <th> "start_time"->React.string </th>
             <th> "status"->React.string </th>
           </tr>
           {data.bridge_sync
            ->Belt.Array.map(({id, end_time, index, start_time, status}) => {
                <tr>

                    <td> {id->string_of_int->React.string} </td>
                    <td>
                      {end_time
                       ->Option.mapWithDefault("in progress", string_of_int)
                       ->React.string}
                    </td>
                    <td> {index->string_of_int->React.string} </td>
                    <td> {start_time->string_of_int->React.string} </td>
                    <td> status->React.string </td>
                    <td
                      // <CountDown
                      //   displayUnits=false
                      //   endDateMoment={MomentRe.momentWithUnix(
                      //     next_scheduled_sync->Option.getWithDefault(0),
                      //   )}
                      // />
                    />
                  </tr>
                  // let numberOfSyncs = getMaxIndexSyncFromAgregate(aggregate);
                  //   Frequency.secondsToText(frequency_duration_seconds);
                  // let frequencyText =
              })
            ->React.array}
         </>
       | {loading: false, data: None} =>
         <p> "Error loading data"->React.string </p>
       }}
    </table>
  </div>;
};
