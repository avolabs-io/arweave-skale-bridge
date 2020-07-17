open Globals;

Dotenv.config();
[@bs.val] external appId: string = "process.env.REACT_APP_LOGIN_APP_ID";

/*
 LOGIN TREE - display tree on initial page load:

   Has checked if the user is already logged in?
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
 */

/*
 mutation MyMutation {
   insert_bridge_data_one(object: {label: "test", active: true, arweaveEndpointId: "test", contentType: "snapshot", frequency: "hourly", metaData: "{}", skaleEndpointId: "test", status: "InProgress", userId: "denhampreen"}) {
     id
   }
 }
 */
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
let useGetSkaleEndpointsQueryResult = userId =>
  GetUserSkaleEndpointsQuery.use(
    ~fetchPolicy=CacheAndNetwork,
    ~errorPolicy=All,
    GetUserSkaleEndpointsQuery.makeVariables(~userId, ()),
  );

module HasLoadedUsersBridgeData = {
  [@react.component]
  let make = (~children) => {
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let endpointQueryResult =
      useGetSkaleEndpointsQueryResult(usersIdDetails.login);
    switch (endpointQueryResult) {
    | {loading: true, data: None} =>
      <p> "Denham - please make a nice loader here"->React.string </p>
    | {loading: _, data: Some(data), error: _} =>
      switch (data.skale_endpoint) {
      | [||] => <> "Start the onboarding process"->React.string </>
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
      <h1>
        "Loading users data. DENHAM - I leave it up to you to put a proper login loader of your choice here :)"
        ->React.string
      </h1>
    };
  };
};

[@react.component]
let make = (~children) => {
  let authObject = OneGraph.initialize({appId: appId});
  let isLoggedIn = RootProvider.useIsLoggedIn();
  let performLogin = RootProvider.useLogin();
  let (isLoadingAuth, setIsLoadingAuth) = React.useState(_ => true);
  let isLoadingData = !isLoggedIn || isLoadingAuth;

  let recordLogin = () => {
    Js.Promise.then_(
      loginStatus => {
        (
          if (loginStatus) {
            setIsLoadingAuth(_ => false);
            performLogin(authObject.authHeaders(.));
          } else {
            ();
          }
        )
        ->async
      },
      authObject.isLoggedIn(. "github"),
    );
  };

  // // Log the user in if they aren't logged in yet:
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
    Js.Promise.then_(_ => {recordLogin()}, authObject.login(. "github"))
    ->ignore;
    ();
  };

  <div className="login">
    {if (isLoadingData) {
       <h1>
         "Loading previous logins - DENHAM - I leave it up to you to put a proper login loader of your choice here :)"
         ->React.string
       </h1>;
     } else if (isLoggedIn) {
       <UserProfileDataLoader> children </UserProfileDataLoader>;
     } else {
       <>
         <div> <img src="./assets/skale-arweave.svg" /> </div>
         <button onClick className="login-button">
           "LOGIN WITH GITHUB"->React.string
           <img src="./assets/github.svg" className="gh-icon" />
         </button>
       </>;
     }}
  </div>;
};
