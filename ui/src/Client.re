let headers = {"x-hasura-admin-secret": "testing"};
// let headers = {"X-Hasura-User-ID": "jason", "X-Hasura-Role": "user"};

[@bs.val]
external graphqlEndpoint: string = "process.env.REACT_APP_GRAPHQL_ENDPOINT";
[@bs.val] external isHttpsEnv: string = "process.env.REACT_APP_IS_HTTPS";
let isHttps = isHttpsEnv == "true";

let httpLink =
  ApolloClient.Link.HttpLink.make(
    ~uri=_ => (isHttps ? "https://" : "http://") ++ graphqlEndpoint,
    ~credentials="include",
    ~headers=Obj.magic(headers),
    (),
  );

let wsLink =
  ApolloClient.Link.WebSocketLink.(
    make(
      ~uri=(isHttps ? "wss://" : "ws://") ++ graphqlEndpoint,
      ~options=
        ClientOptions.make(
          ~connectionParams=
            ConnectionParams(Obj.magic({"headers": headers})),
          ~reconnect=true,
          (),
        ),
      (),
    )
  );

let terminatingLink =
  ApolloClient.Link.split(
    ~test=
      ({query}) => {
        let definition = ApolloClient.Utilities.getOperationDefinition(query);
        switch (definition) {
        | Some({kind, operation}) =>
          kind === "OperationDefinition" && operation === "subscription"
        | None => false
        };
      },
    ~whenTrue=wsLink,
    ~whenFalse=httpLink,
  );

let instance =
  ApolloClient.(
    make(
      ~cache=Cache.InMemoryCache.make(),
      ~connectToDevTools=true,
      ~defaultOptions=
        DefaultOptions.make(
          ~mutate=
            DefaultMutateOptions.make(
              ~awaitRefetchQueries=true,
              ~fetchPolicy=NetworkOnly,
              ~errorPolicy=All,
              (),
            ),
          ~query=
            DefaultQueryOptions.make(
              ~fetchPolicy=NetworkOnly,
              ~errorPolicy=All,
              (),
            ),
          ~watchQuery=
            DefaultWatchQueryOptions.make(
              ~fetchPolicy=NetworkOnly,
              ~errorPolicy=All,
              (),
            ),
          (),
        ),
      ~link=terminatingLink,
      (),
    )
  );
