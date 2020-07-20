open Globals;

module GetUserBridgesQuery = [%graphql
  {|
  query BridgesQuery($userId: String!) {
    bridge_data (where: {user_id: {_eq: $userId}}){
      userId
      id
      contentType
    }
  }
|}
];

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
      <tr>
        <th> "id"->React.string </th>
        <th> "label"->React.string </th>
        <th> "last backed up"->React.string </th>
      </tr>
      <tr>
        <td> "1"->React.string </td>
        <td> "my daily backup"->React.string </td>
        <td> "just now"->React.string </td>
      </tr>
      {switch (usersBridgesQueryResult) {
       | {loading: true, data: None} => <p> "Loading"->React.string </p>
       | {loading, data: Some(data), error} =>
         <>
           <dialog>
             {loading ? <p> "Refreshing..."->React.string </p> : React.null}
             {switch (error) {
              | Some(_) =>
                <p>
                  "The query went wrong, data may be incomplete"->React.string
                </p>
              | None => React.null
              }}
           </dialog>
           {data.bridge_data
            ->Belt.Array.map(bridge =>
                <tr>
                  <td> bridge.contentType->React.string </td>
                  <td> "my daily backup"->React.string </td>
                  <td> "just now"->React.string </td>
                </tr>
              )
            ->React.array}
         </>
       | {loading: false, data: None} =>
         <p> "Error loading data"->React.string </p>
       }}
    </table>
  </div>;
};
