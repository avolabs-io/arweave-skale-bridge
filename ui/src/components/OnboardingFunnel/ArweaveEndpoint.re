[%raw "require('../../styles/css/radio-select.css')"];
open Globals;

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

module EditArweaveEndpoint = {
  [@react.component]
  let make = () => {
    // TODO: allow removing endpoints.
    let (mutate, result) = AddArweaveEndpointMutation.use();
    let (newArweaveEndpoint, setNewArweaveEndpoint) =
      React.useState(() => "");
    let (newArweaveProtocol, setNewArweaveProtocol) =
      React.useState(() => "https");
    let (newArweavePort, setNewArweavePort) = React.useState(() => None);
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();

    let onSubmit = _ =>
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
    switch (result) {
    | {called: false} =>
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
      </form>
    | {loading: true} => "Loading..."->React.string
    | {data: _, error: None} =>
      // Some({uri: Some(uri)
      <h4>
        {React.string("Endpoint has successfully been added to your account.")}
      </h4>
    | {error} =>
      <>
        "Error loading data"->React.string
        {switch (error) {
         | Some(error) => React.string(": " ++ error.message)
         | None => React.null
         }}
      </>
    };
  };
};

module ArweaveEndpointDropdown = {
  [@react.component]
  let make = (~arveaweEndpointInput, ~setArweaveEndpointInput) => {
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
             // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
             //  <select
             //    name="arweaveEndpoint"
             //    id="arweaveEndpoint"
             //    value=selectedArweaveEndpoint
             //    onChange={event => {
             //      let value = ReactEvent.Form.target(event)##value;
             //      setSelectedArweaveEndpoint(_ => value);
             //    }}>
             //    {data.arweave_endpoint
             //     ->Belt.Array.map(endpoint =>
             //         <option value={endpoint.url}>
             //           endpoint.url->React.string
             //         </option>
             //       )
             //     ->React.array}
             //  </select>
             <div className="radio-box-container">
               <ul>
                 {data.arweave_endpoint
                  ->Belt.Array.map(({id, url, port, protocol}) =>
                      <li
                        onChange={_event => {
                          setArweaveEndpointInput(_ => Some(id))
                        }}>
                        <input
                          type_="radio"
                          id={id->string_of_int ++ "-option"}
                          name="selector"
                          value=url
                          checked={
                            id
                            == arveaweEndpointInput->Option.getWithDefault(-1)
                          }
                        />
                        <label htmlFor={id->string_of_int ++ "-option"}>
                          {(
                             protocol
                             ++ "://"
                             ++ url
                             ++ ":"
                             ++ port->string_of_int
                           )
                           ->React.string}
                        </label>
                        <div className="check" />
                      </li>
                    )
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
      ~arveaweEndpointInput,
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
              arveaweEndpointInput
              setArweaveEndpointInput
            />
          }}
         // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
       </>
     | {loading: false, data: None} =>
       <p> "Error loading existing endpoints."->React.string </p>
     }}
    <EditArweaveEndpoint />
    <NavigationButtons moveToNextStep moveToPrevStep />
  </div>;
};
