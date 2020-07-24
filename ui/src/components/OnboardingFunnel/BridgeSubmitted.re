module GetUserArweaveWalletQuery = [%graphql
  {|
  query EndpointQuery ($userId: String!) {
    arweave_key(where: {user_id: {_eq: $userId}}) {
      pub_key
      user_id
    }
  }
|}
];

module CreateBridge = [%graphql
  {|
  mutation MyMutation ($arweaveEndpointId: Int!, $contentType: String!, $frequencyDurationSeconds: Int!, $nextScheduledSync: Int!, $skaleEndpointId: Int!, $userId: String!){
  insert_bridge_data_one(object: {arweave_endpoint_id: $arweaveEndpointId, contentType: $contentType, frequency_duration_seconds: $frequencyDurationSeconds, next_scheduled_sync: $nextScheduledSync, skale_endpoint_id: $skaleEndpointId, userId: $userId}) {
    active
  }
}
|}
];

module GenerateArweaveWallet = {
  [@react.component]
  let make = (~setArweaveAddress) => {
    // TODO: allow removing endpoints.
    let (mutate, result) = CreateBridge.use();
    // let (newArweaveEndpoint, setNewArweaveEndpoint) =
    //   React.useState(() => "");
    // let (newArweaveProtocol, setNewArweaveProtocol) =
    //   React.useState(() => "https");
    // let (newArweavePort, setNewArweavePort) = React.useState(() => None);
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();

    React.useEffect(() => {
      mutate(
        ~refetchQueries=[|
          GetUserArweaveWalletQuery.refetchQueryDescription(
            GetUserArweaveWalletQuery.makeVariables(
              ~userId=usersIdDetails.login,
              (),
            ),
          ),
        |],
        CreateBridge.makeVariables(
          ~arweaveEndpointId=5,
          ~contentType="something",
          ~frequencyDurationSeconds=5,
          ~nextScheduledSync=5,
          ~skaleEndpointId=5,
          ~userId=usersIdDetails.login,
          (),
        ),
      )
      ->ignore;
      None;
    });
    CreateBridge.(
      switch (result) {
      | {called: false} => React.null
      | {loading: true} => "Loading..."->React.string
      | {data, error: None} =>
        switch (data) {
        // | Some(newAddress) =>
        | Some({createArweaveWallet: Some({address})}) =>
          setArweaveAddress(_ => Some(address));

          <h4>
            {React.string(
               "Your Arweave Account has been created. " ++ address,
             )}
          </h4>;
        | _ =>
          <h4>
            {React.string(
               "There was an error creating your arweave address. Please reload.",
             )}
          </h4>
        }
      | {error} =>
        <>
          "Error creating arweave account."->React.string
          {switch (error) {
           | Some(error) => React.string(": " ++ error.message)
           | None => React.null
           }}
        </>
      }
    );
  };
};

[@react.component]
let make = (~moveToNextStep, ~moveToPrevStep, ~setArweaveAddress) => {
  let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
  let arweaveEndpointsQueryResult =
    GetUserArweaveWalletQuery.use(
      ~fetchPolicy=CacheAndNetwork,
      ~errorPolicy=All,
      GetUserArweaveWalletQuery.makeVariables(
        ~userId=usersIdDetails.login,
        (),
      ),
    );
  <div className="funnel-step-container">
    <h2>
      "Please deposit into your arweave account so that we can perform syncs on your behalf."
      ->React.string
    </h2>
    {switch (arweaveEndpointsQueryResult) {
     | {loading: true, data: None} => React.null
     | {loading, data: Some(data), error} =>
       <>
         {loading
            ? <p> "Loading your current data."->React.string </p> : React.null}
         {switch (error) {
          | Some(_) =>
            <p>
              "Unexpected error, please check your connection."->React.string
            </p>
          | None => React.null
          }}
         {switch (data.arweave_key) {
          | [||] => <GenerateArweaveWallet setArweaveAddress />
          | [|{pub_key}|]
          | [|{pub_key}, _|] =>
            setArweaveAddress(_ => Some(pub_key));
            <div>
              <p> "Your arweave wallet address:"->React.string </p>
              <p> pub_key->React.string </p>
            </div>;
          }}
         // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
       </>
     | {loading: false, data: None} =>
       <p> "Error loading existing endpoints."->React.string </p>
     }}
    <NavigationButtons moveToNextStep moveToPrevStep />
  </div>;
};
