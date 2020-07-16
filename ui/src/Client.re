let headers = {
  "x-hasura-admin-secret": "testing",
  "Authorization": "Bearer eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6Im8xdDFoQXk2Z3VreDgxZXFVQU54VXE4WjRuSSJ9.eyJpc3MiOiJPbmVHcmFwaCIsImF1ZCI6Imh0dHBzOi8vc2VydmUub25lZ3JhcGguY29tL2Rhc2hib2FyZC9hcHAvODBkMjg3ZjUtYTUxNC00NmNhLWExN2EtY2FhMjYzYjhlMmNiIiwiaWF0IjoxNTk0NzQyODE1LCJleHAiOjE1OTU5NTI0MTUsImh0dHBzOi8vb25lZ3JhcGguY29tL2p3dC9jbGFpbXMiOnsiYWNjZXNzX3Rva2VuIjoiT0pxdGhGUlB3Z0VpaWw1YVZnY2g5S3hYTTVGZi1qTkpQNzFuWDBMQXBWTSJ9LCJodHRwczovL2hhc3VyYS5pby9qd3QvY2xhaW1zIjp7IngtaGFzdXJhLWRlZmF1bHQtcm9sZSI6InVzZXIiLCJ4LWhhc3VyYS1hbGxvd2VkLXJvbGVzIjpbInVzZXIiXX0sImFwcF9tZXRhZGF0YSI6eyJhdXRob3JpemF0aW9uIjp7InJvbGVzIjpbInVzZXIiXX19fQ.B7lZZ-lF831LzdamecyFJddXPfSSwl6G6OsN8HDxFtLMKhD9xqHjAHM_Xkr0WwcsiZStjoue3mMttkQ3s07VTEnH1ukr4l7ydeRYzyR7sg6b6fCri7qSKUaKMkEXoFtXG2p53n3fIUhKUXgf2VgWrVunb7efS1zKkTiN8qoofB6Y0-j7ZjevNL2kqyBhBWgOAbkgEAq0hQHwlk4HwI3wNRRRTCIJLbQ1gS4TK3-bRe1pRLo4FdAiUSt_g9CoqceXPd18ICHMC1nmCopHwyevent-x8lkPYgfU63kCznqs2LlLhH9RhqzC9d4bbo9lAcnVdGKegVolFPa3uciPBD3pA",
};
// let headers = {"X-Hasura-User-ID": "jason", "X-Hasura-Role": "user"};

[@bs.val]
external graphqlEndpoint: string = "process.env.REACT_APP_GRAPHQL_ENDPOINT";
[@bs.val] external isHttpsEnv: string = "process.env.REACT_APP_IS_HTTPS";
let isHttps = isHttpsEnv == "true";

let httpLink = headers =>
  ApolloClient.Link.HttpLink.make(
    ~uri=_ => (isHttps ? "https://" : "http://") ++ graphqlEndpoint,
    ~credentials="include",
    ~headers=Obj.magic(headers),
    (),
  );

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

// let instance =
//   ApolloClient.(
//     make(
//       ~cache=Cache.InMemoryCache.make(),
//       ~connectToDevTools=true,
//       ~defaultOptions=
//         DefaultOptions.make(
//           ~mutate=
//             DefaultMutateOptions.make(
//               ~awaitRefetchQueries=true,
//               ~fetchPolicy=NetworkOnly,
//               ~errorPolicy=All,
//               (),
//             ),
//           ~query=
//             DefaultQueryOptions.make(
//               ~fetchPolicy=NetworkOnly,
//               ~errorPolicy=All,
//               (),
//             ),
//           ~watchQuery=
//             DefaultWatchQueryOptions.make(
//               ~fetchPolicy=NetworkOnly,
//               ~errorPolicy=All,
//               (),
//             ),
//           (),
//         ),
//       ~link=terminatingLink,
//       (),
//     )
//   );

let useGlobalApolloInstance = () => {
  let headers = RootProvider.useHeaders();
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
    [|headers|],
  );
};
