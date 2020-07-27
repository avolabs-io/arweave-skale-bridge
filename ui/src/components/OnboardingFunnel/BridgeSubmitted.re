open Globals;

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

open OnboardingHelpers;

module CreateBridgeForm = {
  [@react.component]
  let make =
      (
        ~skaleEndpointInput,
        ~skaleDataTypeInput,
        ~frequencyInput,
        ~arweaveEndpointInput,
        ~setHasCreatedBridge,
      ) => {
    let (mutate, result) = CreateBridge.use();

    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let createBridge = () => {
      mutate(
        CreateBridge.makeVariables(
          ~arweaveEndpointId=arweaveEndpointInput,
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

    <div className="funnel-step-container frequency radio-box-container">
      {switch (result) {
       | {called: false} =>
         <>
           <HiddenAutoFocusButton action={_ => createBridge()->ignore} />
           <h1> "Do you want to create your bridge now?"->React.string </h1>
           <button
             onClick={_ => createBridge()->ignore}
             className="confirm-bridge-button">
             "Confirm"->React.string
           </button>
         </>
       | {loading: true} => "Loading..."->React.string
       | {data, error: None} =>
         switch (data) {
         | Some(_mutationResult) =>
           setHasCreatedBridge(_ => true);

           <>
             <HiddenAutoFocusButton
               action={_ => Route.Dashboard->Router.push}
             />
             <h4> {React.string("Your Bridge has been created.")} </h4>
             <Router.Link route=Route.Dashboard>
               "Manage your active Bridges."->React.string
             </Router.Link>
           </>;
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
       }}
    </div>;
  };
};

[@react.component]
let make =
    (
      ~skaleEndpointInput,
      ~skaleDataTypeInput,
      ~frequencyInput,
      ~arweaveEndpointInput,
      ~goToStep,
    ) => {
  let (_hasCreatedBridge, setHasCreatedBridge) = React.useState(_ => false);

  switch (
    skaleDataTypeInput,
    skaleEndpointInput,
    frequencyInput,
    arweaveEndpointInput,
  ) {
  | (
      Some(skaleDataTypeInput),
      Some(skaleEndpointInput),
      Some(frequencyInput),
      Some(arweaveEndpointInput),
    ) =>
    <CreateBridgeForm
      skaleDataTypeInput
      skaleEndpointInput
      frequencyInput
      arweaveEndpointInput
      setHasCreatedBridge
    />
  | _ =>
    <>
      {skaleDataTypeInput->Option.mapWithDefault(
         <p>
           "The skale data type isn't defined."->React.string
           <a onClick={_ => goToStep(Route.SkaleDataTypeToStore)}>
             "Please go back and define it."->React.string
           </a>
         </p>,
         _ =>
         React.null
       )}
      {skaleEndpointInput->Option.mapWithDefault(
         <p>
           "The skale data type isn't defined."->React.string
           <a onClick={_ => goToStep(Route.SkaleEndpoint)}>
             "Please go back and define it."->React.string
           </a>
         </p>,
         _ =>
         React.null
       )}
      {frequencyInput->Option.mapWithDefault(
         <p>
           "The frequency of your bridge backup isn't defined."->React.string
           <a onClick={_ => goToStep(Route.Frequency)}>
             "Please go back and define it."->React.string
           </a>
         </p>,
         _ =>
         React.null
       )}
      {arweaveEndpointInput->Option.mapWithDefault(
         <p>
           "The Arveawe Endpoint isn't defined."->React.string
           <a onClick={_ => goToStep(Route.ArweaveEndpoint)}>
             "Please go back and define it."->React.string
           </a>
         </p>,
         _ =>
         React.null
       )}
    </>
  };
};
