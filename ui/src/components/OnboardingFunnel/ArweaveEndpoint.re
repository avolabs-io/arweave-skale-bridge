[%raw "require('../../styles/css/radio-select.css')"];
open Globals;
open OnboardingHelpers;

// TODO: enforce that the "user <-> uri" combination is unique. https://github.com/hasura/graphql-engine/issues/2200
//       I had no luck first time I tried.
module AddArweaveEndpointMutation = [%graphql
  {|
    mutation addArweaveEndpoint($url: String!, $protocol: String!, $port: Int!, $userId: String!) {
      insert_arweave_endpoint_one(object: {url: $url, user_id: $userId, protocol: $protocol, port: $port}) {
        id
        url
        user_id
        protocol
        port
      }
    }
  |}
];

module GetUserArweaveEndpointsQuery = [%graphql
  {|
  query EndpointQuery($userId: String!) {
    arweave_endpoint (where: {user_id: {_eq: $userId}}){
      url
      user_id
      id
      protocol
      port
    }
  }
|}
];

let formatArweaveUrl = (protocol, host, port) => {
  protocol ++ "://" ++ host ++ ":" ++ port->string_of_int;
};

module EditArweaveEndpoint = {
  [@react.component]
  let make = (~setArweaveEndpointInput, ~goToNextStep) => {
    let (mutate, result) = AddArweaveEndpointMutation.use();
    let (newArweaveEndpoint, setNewArweaveEndpoint) =
      React.useState(() => "");
    let (newArweaveProtocol, setNewArweaveProtocol) =
      React.useState(() => "https");
    let (newArweavePort, setNewArweavePort) = React.useState(() => None);
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let ((isValidating, isValid, message), setValidatingEndpoint) =
      React.useState(() => (false, true, None));
    let onSubmit = _ =>
      {
        setValidatingEndpoint(_ => (true, true, None));
        CheckLink.checkIsValidLink(
          formatArweaveUrl(
            newArweaveProtocol,
            newArweaveEndpoint,
            newArweavePort->Option.getWithDefault(80),
          ),
          false,
        )
        ->Js.Promise.then_(
            ((isValid, message)) => {
              setValidatingEndpoint(_ => (false, isValid, message));
              if (isValid) {
                mutate(
                  ~refetchQueries=[|
                    GetUserArweaveEndpointsQuery.refetchQueryDescription(
                      GetUserArweaveEndpointsQuery.makeVariables(
                        ~userId=usersIdDetails.login,
                        (),
                      ),
                    ),
                  |],
                  AddArweaveEndpointMutation.makeVariables(
                    ~url=newArweaveEndpoint,
                    ~protocol=newArweaveProtocol,
                    ~port=newArweavePort->Option.getWithDefault(80) /* For now just have the default port be 80... */,
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
          <div
            className=Cn.(
              "input-with-button"
              + Css.(style([display(`flex), justifyContent(`spaceBetween)]))
            )>
            <select
              name="skaleEndpoint"
              id="skaleEndpoint"
              value=newArweaveProtocol
              className=Css.(style([maxWidth(`percent(20.))]))
              onChange={event => {
                let value = ReactEvent.Form.target(event)##value;
                setNewArweaveProtocol(_ => value);
              }}>
              <option value="https"> "https://"->React.string </option>
              <option value="http"> "http://"->React.string </option>
            </select>
            <input
              typeof="text"
              id="newArweaveEndpoint"
              name="newArweaveEndpoint"
              placeholder="Add new host"
              onChange={event => {
                let value = ReactEvent.Form.target(event)##value;
                setNewArweaveEndpoint(_ => value);
              }}
            />
            <input
              typeof="number"
              id="newArweavePort"
              name="newArweavePort"
              value={newArweavePort->Option.mapWithDefault("", string_of_int)}
              placeholder="Port"
              onChange={event => {
                let value = ReactEvent.Form.target(event)##value;
                setNewArweavePort(_ => value->int_of_string_opt);
              }}
              className=Css.(style([maxWidth(`percent(20.))]))
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
      ->Option.flatMap(({insert_arweave_endpoint_one}) =>
          insert_arweave_endpoint_one
        )
      ->Option.map(({id}) => {
          setArweaveEndpointInput(_ => Some(id));
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

module ArweaveEndpointDropdown = {
  [@react.component]
  let make =
      (~arweaveEndpointInput, ~setArweaveEndpointInput, ~moveToNextStep) => {
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();

    let arweaveEndpointsQueryResult =
      GetUserArweaveEndpointsQuery.use(
        ~fetchPolicy=CacheAndNetwork,
        ~errorPolicy=All,
        GetUserArweaveEndpointsQuery.makeVariables(
          ~userId=usersIdDetails.login,
          (),
        ),
      );

    <div>
      <h3> "Select the desired arweave endpoint"->React.string </h3>
      <div>
        {switch (arweaveEndpointsQueryResult) {
         | {loading: true, data: None} => <p> "Loading"->React.string </p>
         | {loading, data: Some({arweave_endpoint}), error} =>
           <>
             <HiddenAutoFocusButton
               action={_ =>
                 switch (arweave_endpoint->List.fromArray) {
                 | [] => ()
                 | [{id}, ..._] =>
                   let idToSet =
                     arweaveEndpointInput->Option.getWithDefault(id);
                   setArweaveEndpointInput(_ => Some(idToSet));
                   Js.Global.setTimeout(moveToNextStep, 500)->ignore;
                 }
               }
             />
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
             <div className="radio-box-container">
               <ul>
                 {arweave_endpoint
                  ->Belt.Array.map(({id, url, port, protocol}) => {
                      let checked = {
                        id == arweaveEndpointInput->Option.getWithDefault(-1);
                      };
                      <li
                        onChange={_event => {
                          setArweaveEndpointInput(_ => Some(id));
                          Js.Global.setTimeout(moveToNextStep, 500)->ignore;
                        }}>
                        <input
                          type_="radio"
                          id={id->string_of_int ++ "-option"}
                          name="selector"
                          value=url
                          checked
                          readOnly=true
                        />
                        <label
                          htmlFor={id->string_of_int ++ "-option"}
                          className={checked ? selectedItemAnimation : ""}>
                          {formatArweaveUrl(protocol, url, port)->React.string}
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
      ~arweaveEndpointInput,
      ~setArweaveEndpointInput,
    ) => {
  let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
  let arweaveEndpointsQueryResult =
    GetUserArweaveEndpointsQuery.use(
      ~fetchPolicy=CacheAndNetwork,
      ~errorPolicy=All,
      GetUserArweaveEndpointsQuery.makeVariables(
        ~userId=usersIdDetails.login,
        (),
      ),
    );
  <div className="funnel-step-container">
    <h2> "Please select or input an arweave endpoint"->React.string </h2>
    {switch (arweaveEndpointsQueryResult) {
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
         {switch (data.arweave_endpoint) {
          | [||] => React.null
          | _ =>
            <ArweaveEndpointDropdown
              arweaveEndpointInput
              setArweaveEndpointInput
              moveToNextStep
            />
          }}
       </>
     | {loading: false, data: None} =>
       <p> "Error loading existing endpoints."->React.string </p>
     }}
    <EditArweaveEndpoint setArweaveEndpointInput goToNextStep=moveToNextStep />
    <NavigationButtons
      moveToNextStep
      moveToPrevStep
      nextDisabled={arweaveEndpointInput->Option.isNone}
    />
  </div>;
};
