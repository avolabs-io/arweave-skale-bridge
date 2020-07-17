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

module EditSkaleEndpoint = {
  [@react.component]
  let make = () => {
    // TODO: allow removing endpoints.
    let (mutate, result) = AddSkaleEndpointMutation.use();
    let (newSkaleEndpoint, setNewSkaleEndpoint) = React.useState(() => "");
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();

    let onSubmit = _ =>
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
    switch (result) {
    | {called: false} =>
      <form onSubmit>
        <input
          typeof="text"
          id="newSkaleEndpoint"
          name="newSkaleEndpoint"
          onChange={event => {
            let value = ReactEvent.Form.target(event)##value;
            setNewSkaleEndpoint(_ => value);
          }}
          value=newSkaleEndpoint
        />
        // "Not called... "->React.string
        <button onClick=onSubmit> "Add Skale Endpoint"->React.string </button>
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

module EndpointDropDown2 = {
  [@react.component]
  let make = () => {
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let (selectedSkaleEndpoint, setSelectedSkaleEndpoint) =
      React.useState(() => "");

    let skaleEndpointsQueryResult =
      GetUserSkaleEndpointsQuery.use(
        ~fetchPolicy=CacheAndNetwork,
        ~errorPolicy=All,
        GetUserSkaleEndpointsQuery.makeVariables(
          ~userId=usersIdDetails.login,
          (),
        ),
      );

    <div className="configuration">
      <h3> "Select the desired skale endpoint"->React.string </h3>
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
             // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
             <select
               name="skaleEndpoint"
               id="skaleEndpoint"
               value=selectedSkaleEndpoint
               onChange={event => {
                 let value = ReactEvent.Form.target(event)##value;
                 setSelectedSkaleEndpoint(_ => value);
               }}>
               {data.skale_endpoint
                ->Belt.Array.map(endpoint =>
                    <option value={endpoint.uri}>
                      endpoint.uri->React.string
                    </option>
                  )
                ->React.array}
             </select>
           </>
         | {loading: false, data: None} =>
           <p> "Error loading data"->React.string </p>
         }}
      </div>
    </div>;
  };
};

module SkaleEndpointDropdown = {
  [@react.component]
  let make =
      (~skaleEndpoints: array(GetUserSkaleEndpointsQuery.t_skale_endpoint)) => {
    <select
      name="skaleEndpoint"
      id="skaleEndpoint"
      value="some value" //selectedSkaleEndpoint
      onChange={event => {
        let value = ReactEvent.Form.target(event)##value;
        Js.log(value);
        ();
      }}>
      //  setSelectedSkaleEndpoint(_ => value);

        {skaleEndpoints
         ->Belt.Array.map(endpoint
             //  <option value="endpoint"> "endpoint.uri"->React.string </option>
             =>
               <option value={endpoint.uri}>
                 endpoint.uri->React.string
               </option>
             )
         ->React.array}
      </select>;
  };
};

[@react.component]
let make = (~moveToNextStep) => {
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
    <h2> "Please input a skale endpoint"->React.string </h2>
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
          | skaleEndpoints => <SkaleEndpointDropdown skaleEndpoints />
          }}
         // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
       </>
     | {loading: false, data: None} =>
       <p> "Error loading existing endpoints."->React.string </p>
     }}
    <EditSkaleEndpoint />
    <button onClick=moveToNextStep> "Next"->React.string </button>
  </div>;
};
