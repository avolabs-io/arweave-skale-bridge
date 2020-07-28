[%raw "require('../../styles/css/radio-select.css')"];
[%raw "require('../../styles/css/create-stream.css')"];
open Globals;
open OnboardingHelpers;

// TODO: enforce that the "user <-> uri" combination is unique. https://github.com/hasura/graphql-engine/issues/2200
//       I had no luck first time I tried.
module AddSkaleEndpointMutation = [%graphql
  {|
    mutation addSkaleEndpoint($uri: String!, $userId: String!, $chainId: Int!) {
      insert_skale_endpoint_one(object: {uri: $uri, user_id: $userId, chain_id: $chainId}) {
        id
        uri
        user_id
        chain_id
      }
    }
  |}
];

module GetUserSkaleEndpointsQuery = [%graphql
  {|
  query EndpointQuery($userId: String!) {
    skale_endpoint (where: {user_id: {_eq: $userId}}){
      uri
      user_id
      id
      chain_id
    }
  }
|}
];

module GetSkaleEndpointsByIdQuery = [%graphql
  {|
  query EndpointInfoQuery($endpointId: Int!) {
    skale_endpoint_by_pk(id: $endpointId) {
      uri
      user_id
      id
    }
  }
|}
];

module EditSkaleEndpoint = {
  [@react.component]
  let make = (~setSkaleEndpointInput, ~goToNextStep) => {
    // TODO: allow removing endpoints.
    let (mutate, result) = AddSkaleEndpointMutation.use();
    let (newSkaleEndpoint, setNewSkaleEndpoint) = React.useState(() => "");
    let (newSkaleEndpointId, setNewSkaleEndpointId) =
      React.useState(() => None);
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let ((isValidating, isValid, message), setValidatingEndpoint) =
      React.useState(() => (false, true, None));

    let handleSubmit = () =>
      {
        setValidatingEndpoint(_ => (true, true, None));
        switch (newSkaleEndpointId) {
        | None =>
          Js.log("chain id is undefined!");
          setValidatingEndpoint(_ =>
            (
              false,
              false,
              Some("You must specify a chain ID for your skale endpoint"),
            )
          );
        | Some(chainId) =>
          Js.log("the chain id is defined");
          CheckLink.checkIsValidLink(newSkaleEndpoint, false)
          ->Js.Promise.then_(
              ((isValid, message)) => {
                setValidatingEndpoint(_ => (false, isValid, message));
                if (isValid) {
                  mutate(
                    ~refetchQueries=[|
                      GetUserSkaleEndpointsQuery.refetchQueryDescription(
                        GetUserSkaleEndpointsQuery.makeVariables(
                          ~userId=usersIdDetails.login,
                          (),
                        ),
                      ),
                    |],
                    AddSkaleEndpointMutation.makeVariables(
                      ~uri=newSkaleEndpoint,
                      ~userId=usersIdDetails.login,
                      ~chainId,
                      (),
                    ),
                  )
                  ->ignore;
                } else {
                  ();
                };
                ()->async;
              },
              _,
            )
          ->ignore;
        };
      }
      ->ignore;

    let inputForm =
      if (isValidating) {
        <div>
          <p>
            "Validating your link - this can take some time depending on the network response time."
            ->React.string
          </p>
        </div>;
      } else {
        <form
          onSubmit={event => {
            event->ReactEvent.Form.preventDefault;
            handleSubmit();
          }}>
          <div
            className=Cn.(
              "input-with-button"
              + Css.(style([display(`flex), justifyContent(`spaceBetween)]))
            )>
            <input
              typeof="text"
              id="newSkaleEndpoint"
              name="newSkaleEndpoint"
              placeholder="Add new endpoint"
              onChange={event => {
                let value = ReactEvent.Form.target(event)##value;
                setNewSkaleEndpoint(_ => value);
              }}
              onFocus={_ => {setSkaleEndpointInput(_ => None)}}
              value=newSkaleEndpoint
              className=Css.(style([maxWidth(`percent(70.))]))
            />
            <input
              typeof="number"
              id="newSkaleEndpointId"
              name="newSkaleEndpointId"
              value={
                newSkaleEndpointId->Option.mapWithDefault("", string_of_int)
              }
              placeholder="Chain id"
              onChange={event => {
                let value = ReactEvent.Form.target(event)##value;
                setNewSkaleEndpointId(_ => value->int_of_string_opt);
              }}
              className=Css.(style([maxWidth(`percent(30.))]))
            />
            <button
              onClick={event => {
                event->ReactEvent.Mouse.preventDefault;
                handleSubmit();
              }}
              className="input-add-button">
              "+"->React.string
            </button>
          </div>
          {isValid
             ? React.null
             : <>
                 <p>
                   "Please make sure your input is correct:"->React.string
                 </p>
                 {switch (message) {
                  | Some(errorMessage) => <p> errorMessage->React.string </p>
                  | None => React.null
                  }}
               </>}
        </form>;
      };

    switch (result) {
    | {called: false} => inputForm
    | {loading: true} => "Loading..."->React.string
    | {data, error: None} =>
      data
      ->Option.flatMap(({insert_skale_endpoint_one}) =>
          insert_skale_endpoint_one
        )
      ->Option.map(({id}) => {
          setSkaleEndpointInput(_ => Some(id));
          Js.Global.setTimeout(goToNextStep, 900)->ignore;
        })
      ->ignore;

      <h4>
        {React.string("Endpoint has successfully been added to your account.")}
      </h4>;
    | {error} =>
      <>
        "Error setting your endpoint"->React.string
        {switch (error) {
         | Some(error) => React.string(": " ++ error.message)
         | None => React.null
         }}
        <p> "Please try again:"->React.string </p>
        inputForm
      </>
    };
  };
};

module SkaleEndpointDropDown = {
  [@react.component]
  let make = (~setSkaleEndpointInput, ~skaleEndpointInput, ~moveToNextStep) => {
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();

    let skaleEndpointsQueryResult =
      GetUserSkaleEndpointsQuery.use(
        ~fetchPolicy=CacheAndNetwork,
        ~errorPolicy=All,
        GetUserSkaleEndpointsQuery.makeVariables(
          ~userId=usersIdDetails.login,
          (),
        ),
      );

    <div className="radio-box-container">
      {switch (skaleEndpointsQueryResult) {
       | {loading: true, data: None} => <p> "Loading"->React.string </p>
       | {loading, data: Some({skale_endpoint}), error} =>
         <>
           <HiddenAutoFocusButton
             action={_ =>
               switch (skale_endpoint->List.fromArray) {
               | [] => ()
               | [{id}, ..._] =>
                 let idToSet = skaleEndpointInput->Option.getWithDefault(id);
                 setSkaleEndpointInput(_ => Some(idToSet));
                 Js.Global.setTimeout(moveToNextStep, 500)->ignore;
               }
             }
           />
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
           <div>
             <ul>
               {skale_endpoint
                ->Belt.Array.map(({id, uri, chain_id}) => {
                    let checked =
                      id == skaleEndpointInput->Option.getWithDefault(-1);
                    let idString = id->string_of_int;
                    <li
                      key=idString
                      onClick={_ => {
                        setSkaleEndpointInput(_ => Some(id));
                        // TODO: the timoout should be canceled if another option is selected.
                        Js.Global.setTimeout(moveToNextStep, 500)->ignore;
                      }}>
                      <input
                        type_="radio"
                        id={idString ++ "-option"}
                        name="selector"
                        value=uri
                        onKeyDown={_ => {
                          %log.warn
                          "KeyDown"
                        }}
                        checked
                        readOnly=true
                      />
                      <label
                        htmlFor={idString ++ "-option"}
                        className={checked ? selectedItemAnimation : ""}>
                        {(
                           uri
                           ++ " (chain id: "
                           ++ chain_id->string_of_int
                           ++ ")"
                         )
                         ->React.string}
                      </label>
                      <div className="check" />
                    </li>;
                  })
                ->React.array}
             </ul>
           </div>
         </>
       | {loading: false, data: None} =>
         <p> "Error loading data"->React.string </p>
       }}
    </div>;
  };
};

[@react.component]
let make =
    (
      ~moveToNextStep,
      ~moveToPrevStep,
      ~setSkaleEndpointInput,
      ~skaleEndpointInput,
    ) => {
  let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
  let skaleEndpointsQueryResult =
    GetUserSkaleEndpointsQuery.use(
      ~fetchPolicy=CacheAndNetwork,
      ~errorPolicy=All,
      GetUserSkaleEndpointsQuery.makeVariables(
        ~userId=usersIdDetails.login,
        (),
      ),
    );
  <div className="funnel-step-container">
    <h2> "Skale Endpoint"->React.string </h2>
    <h4> "Please select or input a new skale endpoint"->React.string </h4>
    {switch (skaleEndpointsQueryResult) {
     | {loading: true, data: None} => React.null
     | {loading, data: Some(data), error} =>
       <>
         {loading ? <p> "Saving..."->React.string </p> : React.null}
         {switch (error) {
          | Some(_) =>
            <p>
              "Unexpected error, please check your connection"->React.string
            </p>
          | None => React.null
          }}
         {switch (data.skale_endpoint) {
          | [||] => React.null
          | _ =>
            <SkaleEndpointDropDown
              moveToNextStep
              setSkaleEndpointInput
              skaleEndpointInput
            />
          }}
       </>
     | {loading: false, data: None} =>
       <p> "Error loading existing endpoints."->React.string </p>
     }}
    <EditSkaleEndpoint setSkaleEndpointInput goToNextStep=moveToNextStep />
    <NavigationButtons
      moveToNextStep
      moveToPrevStep
      nextDisabled={skaleEndpointInput->Option.isNone}
    />
  </div>;
  // </form>
};
