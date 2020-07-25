[%raw "require('../../styles/css/radio-select.css')"];
[%raw "require('../../styles/css/create-stream.css')"];
open Globals;

// TODO: enforce that the "user <-> uri" combination is unique. https://github.com/hasura/graphql-engine/issues/2200
//       I had no luck first time I tried.
module AddSkaleEndpointMutation = [%graphql
  {|
    mutation addSkaleEndpoint($uri: String!, $userId: String!) {
      insert_skale_endpoint_one(object: {uri: $uri, user_id: $userId}) {
        id
        uri
        user_id
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
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let ((isValidating, isValid, message), setValidatingEndpoint) =
      React.useState(() => (false, true, None));

    let onSubmit = _ =>
      {
        setValidatingEndpoint(_ => (true, true, None));
        CheckLink.checkIsValidLink(newSkaleEndpoint, true)
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
        <form onSubmit>
          <div className="input-with-button">
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
            />
            <button onClick=onSubmit className="input-add-button">
              "+"->React.string
            </button>
          </div>
          {isValid
             ? React.null
             : <>
                 <p>
                   "Please make sure your link is correct:"->React.string
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
          goToNextStep();
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
  let make = (~setSkaleEndpointInput, ~skaleEndpointInput) => {
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
      <div>
        {switch (skaleEndpointsQueryResult) {
         | {loading: true, data: None} => <p> "Loading"->React.string </p>
         | {loading, data: Some(data), error} =>
           <>
             <dialog>
               {loading ? <p> "Refreshing..."->React.string </p> : React.null}
               {switch (error) {
                | Some(_) =>
                  <p>
                    "The query went wrong, data may be incomplete"
                    ->React.string
                  </p>
                | None => React.null
                }}
             </dialog>
             <div>
               <ul>
                 {data.skale_endpoint
                  ->Belt.Array.map(({id, uri}) => {
                      let idString = id->string_of_int;
                      <li
                        key=idString
                        onClick={_ => {setSkaleEndpointInput(_ => Some(id))}}>
                        <input
                          type_="radio"
                          id={idString ++ "-option"}
                          name="selector"
                          value=uri
                          checked={
                            id
                            == skaleEndpointInput->Option.getWithDefault(-1)
                          }
                          readOnly=true
                        />
                        <label htmlFor={idString ++ "-option"}>
                          uri->React.string
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
      </div>
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
            <SkaleEndpointDropDown setSkaleEndpointInput skaleEndpointInput />
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
};
