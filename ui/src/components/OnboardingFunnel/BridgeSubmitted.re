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

module CreateBridgeForm = {
  [@react.component]
  let make =
      (
        ~skaleEndpointInput,
        ~skaleDataTypeInput,
        ~frequencyInput,
        ~arveaweEndpointInput,
        ~setHasCreatedBridge,
      ) => {
    let (mutate, result) = CreateBridge.use();

    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let createBridge = () => {
      mutate(
        // TODO: Refetch the users active bridges
        // ~refetchQueries=[|
        //   GetUserArweaveWalletQuery.refetchQueryDescription(
        //     GetUserArweaveWalletQuery.makeVariables(
        //       ~userId=usersIdDetails.login,
        //       (),
        //     ),
        //   ),
        // |],
        CreateBridge.makeVariables(
          ~arweaveEndpointId=arveaweEndpointInput,
          ~contentType=skaleDataTypeInput,
          ~frequencyDurationSeconds=frequencyInput,
          ~nextScheduledSync=5,
          ~skaleEndpointId=skaleEndpointInput,
          ~userId=usersIdDetails.login,
          (),
        ),
      )
      ->ignore;
      None;
    };

    CreateBridge.(
      switch (result) {
      | {called: false} =>
        <button onClick={_ => createBridge()->ignore}>
          "Confirm"->React.string
        </button>
      | {loading: true} => "Loading..."->React.string
      | {data, error: None} =>
        switch (data) {
        | Some(mutationResult) =>
          setHasCreatedBridge(_ => true);

          <h4>
            {React.string("Your Arweave Account has been created. ")}
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
let make =
    (
      ~skaleEndpointInput,
      ~skaleDataTypeInput,
      ~frequencyInput,
      ~arveaweEndpointInput,
    ) => {
  let (hasCreatedBridge, setHasCreatedBridge) = React.useState(_ => false);

  switch (
    skaleDataTypeInput,
    skaleEndpointInput,
    frequencyInput,
    arveaweEndpointInput,
  ) {
  | (
      Some(skaleDataTypeInput),
      Some(skaleEndpointInput),
      Some(frequencyInput),
      Some(arveaweEndpointInput),
    ) =>
    hasCreatedBridge
      ? "Has created Bridge"->React.string
      : <CreateBridgeForm
          skaleDataTypeInput
          skaleEndpointInput
          frequencyInput
          arveaweEndpointInput
          setHasCreatedBridge
        />
  | _ => "All of the required fields are not defined"->React.string
  };
};
