open Globals;

// TODO: enforce that the "user <-> uri" combination is unique. https://github.com/hasura/graphql-engine/issues/2200
//       I had no luck first time I tried.
module AddArweaveEndpointMutation = [%graphql
  {|
    mutation addArweaveEndpoint($url: String!, $userId: String!) {
      insert_arweave_endpoint_one(object: {url: $url, user_id: $userId}) {
        id
        url
        user_id
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
          ~userId=usersIdDetails.login,
          (),
        ),
      )
      ->ignore;
    switch (result) {
    | {called: false} =>
      <form onSubmit>
        <div className="input-with-button">
          <input
            typeof="text"
            id="newArweaveEndpoint"
            name="newArweaveEndpoint"
            placeholder="Add new endpoint"
            onChange={event => {
              let value = ReactEvent.Form.target(event)##value;
              setNewArweaveEndpoint(_ => value);
            }}
            value=newArweaveEndpoint
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
  let make = () => {
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let (selectedArweaveEndpoint, setSelectedArweaveEndpoint) =
      React.useState(() => "");

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
             <div>
               <ul>
                 {data.arweave_endpoint
                  ->Belt.Array.map(endpoint =>
                      <li>
                        <input
                          type_="radio"
                          id={endpoint.id->string_of_int ++ "-option"}
                          name="selector"
                          value={endpoint.url}
                          onChange={event => {
                            let value = ReactEvent.Form.target(event)##value;
                            setSelectedArweaveEndpoint(_ => value);
                          }}
                        />
                        <label
                          htmlFor={endpoint.id->string_of_int ++ "-option"}>
                          endpoint.url->React.string
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
let make = (~moveToNextStep, ~moveToPrevStep) => {
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
          | _ => <ArweaveEndpointDropdown />
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
