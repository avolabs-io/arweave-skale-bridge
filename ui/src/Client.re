[@bs.val]
external graphqlEndpoint: string = "process.env.REACT_APP_GRAPHQL_ENDPOINT";
[@bs.val] external isHttpsEnv: string = "process.env.REACT_APP_IS_HTTPS";
let isHttps = isHttpsEnv == "true";

let httpLink = headers => {
  ApolloClient.Link.HttpLink.make(
    ~uri=_ => (isHttps ? "https://" : "http://") ++ graphqlEndpoint,
    ~credentials="include",
    ~headers=Obj.magic(headers),
    (),
  );
};

let wsLink = headers =>
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

let terminatingLink = headers =>
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
    ~whenTrue=wsLink(headers),
    ~whenFalse=httpLink(headers),
  );

let useGlobalApolloInstance = headers => {
  React.useMemo1(
    () =>
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
          ~link=terminatingLink(headers),
          (),
        )
      ),
    [|[%bs.raw {|headers.Authorization|}]|],
  );
};
