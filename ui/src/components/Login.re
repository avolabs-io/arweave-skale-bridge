open Globals;

Dotenv.config();
[@bs.val] external appId: string = "process.env.REACT_APP_LOGIN_APP_ID";

/*
  LOGIN TREE - display tree on initial page load:

    Has finished checking if the user is already logged in (previously)?
      TRUE ->
        is logged in?
          TRUE ->
            has loaded users data?
              TRUE ->
                has loaded users bridge data?
                  TRUE ->
                    has user got any active bridges?
                      TRUE -> show overwiev screen
                      FALSE -> kick off process for user to define a bridge
                  FALSE -> load bridge data (show loader)
              FALSE -> load users data (show loader)
          FALSE -> Show Login Screen
      FALSE ->
        show a loading screen until "TRUE" (finished checking if user is logged in)
 //  */

/*
 mutation MyMutation {
   insert_bridge_data_one(object: {label: "test", active: true, arweaveEndpointId: "test", contentType: "snapshot", frequency: "hourly", metaData: "{}", skaleEndpointId: "test", status: "InProgress", userId: "denhampreen"}) {
     id
   }
 }
 */

module GetUserBridgesQuery = [%graphql
  {|
  query BridgesQuery($userId: String!) {
    bridge_data (where: {userId: {_eq: $userId}}){
      id
    }
  }
|}
];
// contentType
// userId

let useGetSkaleEndpointsQueryResult = userId =>
  GetUserBridgesQuery.use(
    ~fetchPolicy=CacheAndNetwork,
    ~errorPolicy=All,
    GetUserBridgesQuery.makeVariables(~userId, ()),
  );

module HasLoadedUsersBridgeData = {
  [@react.component]
  let make = (~children) => {
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let endpointQueryResult =
      useGetSkaleEndpointsQueryResult(usersIdDetails.login);
    switch (endpointQueryResult) {
    | {loading: true, data: None} =>
      <Loader _type="Puff" color="#5b7aba" height=80 width=80 />
    | {loading: _, data: Some(data), error: _} =>
      switch (data.bridge_data) {
      | [||] => <Onboarding />
      | _ => <> children </>
      }
    | {loading: false, data: None} =>
      <p> "Error loading data"->React.string </p>
    };
    // children;
  };
};

module UserProfileDataLoader = {
  [@react.component]
  let make = (~children) => {
    let optAuthHeader = RootProvider.useAuth();
    let loadUserData = RootProvider.useLoadProfileInfo();
    let optUserData = RootProvider.useCurrentUserDetails();

    // Load user's info if not already loaded:
    React.useMemo1(
      () => {
        switch (optAuthHeader) {
        | Some(_) => loadUserData()->ignore
        | None => ()
        };
        // TODO: add a way to safely cancel loading user data here if component remounts/unmounts.
        None;
      },
      [|optAuthHeader|],
    )
    ->ignore;

    // TODO: this component doesn't take into consideration that the 'loadUserData' function **could** fail.
    switch (optUserData) {
    | Some(_) =>
      <HasLoadedUsersBridgeData> children </HasLoadedUsersBridgeData>
    | None =>
      <div className="loader-container">
        <Loader _type="Puff" color="#5b7aba" height=80 width=80 />
      </div>
    // <h3> "Loading users data."->React.string </h3> // TODO @jason I think it loads too fast normally to have text here with it. For some of the slower requests we can add text but not on these short ones
    };
  };
};

[@react.component]
let make = (~children, ~loginProtected=true) => {
  let authObject = OneGraph.initialize({appId: appId});
  let isLoggedIn = RootProvider.useIsLoggedIn();
  let isLoading = RootProvider.useIsLoading();
  let setLoading = RootProvider.useSetLoading();
  let hasFetched = RootProvider.useHasFetchedLoginState();
  let setHasFetched = RootProvider.useSetHasFetchedLoginState();
  let performLogin = RootProvider.useLogin();
  // let (hasFetched, setHasFetched) = React.useState(_ => false); // This should probably be in RootProvider

  let recordLogin = () => {
    Js.Promise.then_(
      loginStatus => {
        (
          if (loginStatus) {
            setHasFetched();
            performLogin(authObject.authHeaders(.));
          } else {
            setHasFetched();
          }
        )
        ->async
      },
      authObject.isLoggedIn(. "github"),
    );
  };

  // Log the user in if they aren't logged in yet:
  React.useEffect0(() => {
    if (isLoggedIn) {
      ();
    } else {
      recordLogin()->ignore;
    };
    None;
  });

  // TODO: move more of this logic to the RootProvider.
  let onClick = _ => {
    setLoading();
    Js.Promise.then_(_ => {recordLogin()}, authObject.login(. "github"))
    ->ignore;
    ();
  };

  <div className="login">
    {if (!loginProtected) {
       children;
     } else if (isLoading) {
       <div className="loader-container">
         <Loader _type="Puff" color="#5b7aba" height=80 width=80 />
         <h3> "Logging in..."->React.string </h3>
       </div>;
     } else if (!hasFetched) {
       <div className="loader-container">
         <Loader _type="Puff" color="#5b7aba" height=80 width=80 />
       </div>;
     } else if (isLoggedIn) {
       <UserProfileDataLoader> children </UserProfileDataLoader>;
     } else {
       <div className="login-container">
         <img src="./assets/skale-arweave.svg" className="joint-logo" />
         <button onClick className="login-button">
           "LOGIN WITH GITHUB"->React.string
           <img src="./assets/icons/github.svg" className="gh-icon" />
         </button>
       </div>;
     }}
  </div>;
};
