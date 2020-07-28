open Globals;

module BridgeSyncsQuery = [%graphql
  {|
subscription Syncs($bridgeId: Int!) {
  bridge_sync(where: {bridge_id: {_eq: $bridgeId}}, limit: 10, order_by: [{index: desc}]) {
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
    <h1> {("Bridge: #" ++ bridgeId->string_of_int)->React.string} </h1>
    <table>
      {switch (usersBridgeQueryResult) {
       | {loading: true, data: None} => <p> "Loading"->React.string </p>
       | {loading, data: Some(data), error} =>
         <>
           <tr>
             <th> {js| 🚀 |js}->React.string "#"->React.string </th>
             <th> {js| ⏳ |js}->React.string "Start Time"->React.string </th>
             <th> {js| ⌛ |js}->React.string "End Time"->React.string </th>
             <th> {js| 🧮 |js}->React.string "Sync #"->React.string </th>
             <th> {js| 🛠 |js}->React.string "Status"->React.string </th>
           </tr>
           {data.bridge_sync
            ->Belt.Array.map(({id, end_time, index, start_time, status}) => {
                <tr>
                  <td> {id->string_of_int->React.string} </td>
                  <td>
                    {MomentRe.Moment.format(
                       "LL",
                       MomentRe.momentWithUnix(start_time),
                     )
                     ->React.string}
                    " "->React.string
                    {MomentRe.Moment.format(
                       "LTS",
                       MomentRe.momentWithUnix(start_time),
                     )
                     ->React.string}
                  </td>
                  <td>
                    {switch (end_time) {
                     | Some(endtime) =>
                       <>
                         {(
                            MomentRe.Moment.format(
                              "LL",
                              MomentRe.momentWithUnix(endtime),
                            )
                            ++ " "
                            ++ MomentRe.Moment.format(
                                 "LTS",
                                 MomentRe.momentWithUnix(endtime),
                               )
                          )
                          ->React.string}
                       </>
                     | None => "Pending"->React.string
                     }}
                  </td>
                  <td> {index->string_of_int->React.string} </td>
                  <td> status->React.string </td>
                </tr>
              })
            ->React.array}
         </>
       | {loading: false, data: None} =>
         <p> "Error loading data"->React.string </p>
       }}
    </table>
  </div>;
};
