open Globals;

module GetUserBridgesQuery = [%graphql
  {|
query ActiveBridgesQuery($userId: String!) {
  bridge_data(where: {userId: {_eq: $userId}, active: {_eq: true}}) {
    userId
    id
    contentType
    arweave_endpoint_rel {
      id
      port
      protocol
      url
    }
    bridge_sync_rel_aggregate {
      aggregate {
        max {
          index
        }
      }
    }
    frequency_duration_seconds
    label
    metaData
    skale_endpoint {
      uri
      id
    }
    next_scheduled_sync
  }
}
|}
];

let getMaxIndexSyncFromAgregate =
    (
      maxAgregate:
        option(
          GetUserBridgesQuery.t_bridge_data_bridge_sync_rel_aggregate_aggregate,
        ),
    ) => {
  maxAgregate
  ->Option.flatMap(obj => {obj.max})
  ->Option.flatMap(max => {max.index})
  ->Option.getWithDefault(0);
};

[@react.component]
let make = () => {
  let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();

  let usersBridgesQueryResult =
    GetUserBridgesQuery.use(
      ~fetchPolicy=CacheAndNetwork,
      ~errorPolicy=All,
      GetUserBridgesQuery.makeVariables(~userId=usersIdDetails.login, ()),
    );

    let onClick=(id)=>{_ => {id->Route.Bridge->Router.push}}

  <div id="bridges">
    <h1> "Bridges"->React.string </h1>
    <table>
      {switch (usersBridgesQueryResult) {
       | {loading: true, data: None} => <p> "Loading"->React.string </p>
       | {loading, data: Some(data), error} =>
         <>
           <tr>
             <th> {js| 📁 |js}->React.string " Type"->React.string </th>
             <th>
               {js| 📥 |js}->React.string
               " Skale Endpoint"->React.string
             </th>
             <th>
               {js| 📤 |js}->React.string
               " Arweave Endpoint"->React.string
             </th>
             <th> {js| 🕔 |js}->React.string " Frequency"->React.string </th>
             <th> {js| ⏳ |js}->React.string " Next Sync"->React.string </th>
             <th> {js| 🧮 |js}->React.string " # Syncs"->React.string </th>
             <th> {js| 🏷️ |js}->React.string " Label"->React.string </th>
           </tr>
           {data.bridge_data
            ->Belt.Array.map(
                (
                  {
                    id,
                    contentType,
                    skale_endpoint,
                    arweave_endpoint_rel,
                    frequency_duration_seconds,
                    next_scheduled_sync,
                    bridge_sync_rel_aggregate: {aggregate},
                    label,
                    metaData,
                  },
                ) => {
                let (skaleEndpoint, skaleEndpointId) =
                  skale_endpoint->Option.mapWithDefault(
                    ("null", (-1)), ({uri, id}) =>
                    (uri, id)
                  );
                let (arweaveEndpoint, areweaveEndpointId) =
                  arweave_endpoint_rel->Option.mapWithDefault(
                    ("null", (-1)), ({protocol, url: host, port, id}) =>
                    (
                      ArweaveEndpoint.formatArweaveUrl(
                        ~host,
                        ~port,
                        ~protocol,
                      ),
                      id,
                    )
                  );
                let frequencyText =
                  Frequency.secondsToText(frequency_duration_seconds);
                let numberOfSyncs = getMaxIndexSyncFromAgregate(aggregate);
                <tr >
                  <td onClick={onClick(id)}> contentType->React.string </td>
                  <td onClick={onClick(id)}> skaleEndpoint->React.string </td>
                  <td onClick={onClick(id)}> arweaveEndpoint->React.string </td>
                  <td onClick={onClick(id)}>frequencyText->React.string </td>
                  <td onClick={onClick(id)}>
                    <CountDown
                      displayUnits=false
                      endDateMoment={MomentRe.momentWithUnix(
                        next_scheduled_sync->Option.getWithDefault(0),
                      )}
                    />
                  </td>
                  <td onClick={onClick(id)}> {numberOfSyncs->string_of_int->React.string} </td>
                  <td> {label->Option.getWithDefault("")->React.string} </td>
                </tr>;
              })
            ->React.array}
         </>
       | {loading: false, data: None} =>
         <p> "Error loading data"->React.string </p>
       }}
    </table>
  </div>;
};
