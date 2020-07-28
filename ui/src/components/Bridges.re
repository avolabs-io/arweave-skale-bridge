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
      chain_id
    }
    next_scheduled_sync
  }
}
|}
];

module MarkBridgeInactiveQuery = [%graphql
  {|
    mutation markBridgeInactiveQuery($id: Int!) {
    update_bridge_data_by_pk(pk_columns: {id: $id}, _set: {active: false}) {
        id
        active
    }
  }
|}
];

module AddLabelToBridgeQuery = [%graphql
  {|
    mutation addLabelToBridgeQuery($id: Int!, $label: String!) {
    update_bridge_data_by_pk(pk_columns: {id: $id}, _set: {label: $label}) {
        id
        label
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

  let (mutate, result) = MarkBridgeInactiveQuery.use();
  let (mutateLabel, resultLabel) = AddLabelToBridgeQuery.use();

  let (editLabel, setEditLabel) = React.useState(_ => false);
  let (newLabel, setNewLabel) = React.useState(_ => "");

  let onClick = (id, _) => {
    id->Route.Bridge->Router.push;
  };

  let deactivateBridge = id => {
    Js.log(id);
    mutate(
      ~refetchQueries=[|
        GetUserBridgesQuery.refetchQueryDescription(
          GetUserBridgesQuery.makeVariables(~userId=usersIdDetails.login, ()),
        ),
      |],
      MarkBridgeInactiveQuery.makeVariables(~id, ()),
    )
    ->ignore;
  };

  let addLabel = (id, label) => {
    setEditLabel(_ => false);
    Js.log(label);
    mutateLabel(
      ~refetchQueries=[|
        GetUserBridgesQuery.refetchQueryDescription(
          GetUserBridgesQuery.makeVariables(~userId=usersIdDetails.login, ()),
        ),
      |],
      AddLabelToBridgeQuery.makeVariables(~id, ~label, ()),
    )
    ->ignore;
  };

  <div id="bridges">
    <h1 onClick={_ => Route.Bridges->Router.push}>
      "Bridges"->React.string
    </h1>
    {switch (usersBridgesQueryResult) {
     | {loading: true, data: None} => <p> "Loading"->React.string </p>
     | {loading, data: Some(data), error} =>
       <table>
         <thead>
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
             <th>
               {js| 🏗️ |js}->React.string
               " Actions"->React.string
             </th>
           </tr>
         </thead>
         <tbody>
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
                let (skaleEndpoint, skaleEndpointId, chainId) =
                  skale_endpoint->Option.mapWithDefault(
                    ("null", (-1), (-1)), ({uri, id, chain_id}) =>
                    (uri, id, chain_id)
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
                <tr key={id->string_of_int}>
                  <td onClick={onClick(id)}> contentType->React.string </td>
                  <td onClick={onClick(id)}>
                    {(skaleEndpoint ++ ":" ++ chainId->string_of_int)
                     ->React.string}
                  </td>
                  <td onClick={onClick(id)}>
                    arweaveEndpoint->React.string
                  </td>
                  <td onClick={onClick(id)}> frequencyText->React.string </td>
                  <td onClick={onClick(id)}>
                    <CountDown
                      displayUnits=false
                      endDateMoment={MomentRe.momentWithUnix(
                        next_scheduled_sync->Option.getWithDefault(0),
                      )}
                    />
                  </td>
                  <td onClick={onClick(id)}>
                    {numberOfSyncs->string_of_int->React.string}
                  </td>
                  <td>
                    {editLabel
                       ? <div>
                           <input
                             value=newLabel
                             onChange={event => {
                               let value =
                                 ReactEvent.Form.target(event)##value;
                               setNewLabel(_ => value);
                             }}
                           />
                           <button onClick={_ => addLabel(id, newLabel)}>
                             {js|✅|js}->React.string
                           </button>
                         </div>
                       //  <input value={label->Option.getWithDefault("")} />
                       : <>
                           {label->Option.getWithDefault("")->React.string}
                         </>}
                  </td>
                  <td>
                    <button
                      onClick={_ => deactivateBridge(id)}
                      className="bridge-action">
                      {js|🗑️|js}->React.string
                    </button>
                    // onClick={_ => addLabel(id, "my label")}
                    <button
                      onClick={_ => setEditLabel(_ => !editLabel)}
                      className="bridge-action">
                      {js|🏷️|js}->React.string
                    </button>
                  </td>
                </tr>;
              })
            ->React.array}
         </tbody>
       </table>
     | {loading: false, data: None} =>
       <p> "Error loading data"->React.string </p>
     }}
  </div>;
};
