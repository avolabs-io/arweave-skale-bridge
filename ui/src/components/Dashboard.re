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

  <div id="dashboard">
    <h1> "Dashboard"->React.string </h1>
    <table>
      {switch (usersBridgesQueryResult) {
       | {loading: true, data: None} => <p> "Loading"->React.string </p>
       | {loading, data: Some(data), error} =>
         <>
           <tr>
             <th> "Type"->React.string </th>
             <th> "Skale Endpoint"->React.string </th>
             <th> "Arweave Endpoint"->React.string </th>
             <th> "Frequency"->React.string </th>
             <th> "Next Scheduled Sync"->React.string </th>
             <th> "Number Completed Syncs"->React.string </th>
             <th> "Label"->React.string </th>
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
                <tr>
                  <td> contentType->React.string </td>
                  <td> skaleEndpoint->React.string </td>
                  <td> arweaveEndpoint->React.string </td>
                  <td> frequencyText->React.string </td>
                  <td>
                    <CountDown
                      displayUnits=false
                      endDateMoment={MomentRe.momentWithUnix(
                        next_scheduled_sync->Option.getWithDefault(0),
                      )}
                    />
                  </td>
                  <td onClick={_ => {id->Route.Bridge->Router.push}}>
                    {numberOfSyncs->string_of_int->React.string}
                  </td>
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
