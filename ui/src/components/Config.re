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
  query EndpointQuery {
    skale_endpoint {
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

    let onSubmit = _ =>
      mutate(
        ~refetchQueries=[|
          GetUserSkaleEndpointsQuery.refetchQueryDescription(),
        |],
        AddSkaleEndpointMutation.makeVariables(
          ~uri=newSkaleEndpoint,
          ~userId=LoginManager.getUser(),
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

[@react.component]
let make = () => {
  LoginManager.setUser("jasoons"); /* ONLY HERE FOR TESTING PURPOSES */

  // TODO: if the user has zero endpoints maybe it should be true by default?
  let (showEditArweaveEndpoints, setShowEditArweaveEndpoints) =
    React.useState(() => false);
  let (selectedArweaveEndpoint, setSelectedArweaveEndpoint) =
    React.useState(() => "");
  // TODO: if the user has zero endpoints maybe it should be true by default?
  let (showEditSkaleEndpoints, setShowEditSkaleEndpoints) =
    React.useState(() => false);
  let (selectedSkaleEndpoint, setSelectedSkaleEndpoint) =
    React.useState(() => "");

  let skaleEndpointsQueryResult =
    GetUserSkaleEndpointsQuery.use(
      ~fetchPolicy=CacheAndNetwork,
      ~errorPolicy=All,
      GetUserSkaleEndpointsQuery.makeVariables(),
    );

  <div>
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
                  "The query went wrong, data may be incomplete"->React.string
                </p>
              | None => React.null
              }}
           </dialog>
           // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
           <select
             name="scaleEndpoint"
             id="scaleEndpoint"
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
    {if (showEditSkaleEndpoints) {
       <>
         <button onClick={_ => setShowEditSkaleEndpoints(_ => false)}>
           "Finish Editing"->React.string
         </button>
         <EditSkaleEndpoint />
       </>;
     } else {
       <button onClick={_ => setShowEditSkaleEndpoints(_ => true)}>
         "Edit your endpoints"->React.string
       </button>;
     }}
    <h3> "Select the desired arweave endpoint"->React.string </h3>
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
                  "The query went wrong, data may be incomplete"->React.string
                </p>
              | None => React.null
              }}
           </dialog>
           // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
           <select
             name="scaleEndpoint"
             id="scaleEndpoint"
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
    {if (showEditSkaleEndpoints) {
       <>
         <button onClick={_ => setShowEditSkaleEndpoints(_ => false)}>
           "Finish Editing"->React.string
         </button>
         <EditSkaleEndpoint />
       </>;
     } else {
       <button onClick={_ => setShowEditSkaleEndpoints(_ => true)}>
         "Edit your endpoints"->React.string
       </button>;
     }}
  </div>;
};
