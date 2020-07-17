open Globals;

type authHeader = {
  [@bs.as "Authorization"]
  auth: string,
};
module MeQuery = [%graphql
  {|
  query MeQuery {
     user: me {
       github {
         login
         name
         bio
       }
     }
   }
   |}
];

type userDetailsObj = MeQuery.t_user_github;
type rootContextType = {
  isLoggedIn: bool,
  userDetails: option(userDetailsObj),
  authHeader: option(authHeader),
};
type rootActions =
  | LogOut
  | Login(authHeader)
  | LoadDetails(userDetailsObj);

let defaultContext = {isLoggedIn: false, userDetails: None, authHeader: None};

let reducer = (prevState, action) =>
  switch (action) {
  | LogOut => defaultContext
  | Login(authHeader) => {
      ...prevState,
      isLoggedIn: true,
      authHeader: Some(authHeader),
    }
  | LoadDetails(userDetails) => {
      ...prevState,
      userDetails: Some(userDetails),
    }
  };

module RootContext = {
  let context = React.createContext((defaultContext, _ => ()));
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(context);
};

[@react.component]
let make = (~children) => {
  let (rootState, dispatch) = React.useReducer(reducer, defaultContext);

  <RootContext value=(rootState, dispatch)> children </RootContext>;
};

let useLogout: unit => unit =
  () => {
    let (_, dispatch) = React.useContext(RootContext.context);
    dispatch(LogOut);
  };

let useAuth = () => {
  let (state, _) = React.useContext(RootContext.context);
  state.authHeader;
};
let useHeaders = () => {
  switch (useAuth()) {
  | Some(authHeader) => {
      // "x-hasura-role": "user",
      "x-hasura-admin-secret": "testing",
      "Authorization": authHeader.auth,
    }
  | None => {
      "x-hasura-admin-secret": "testing",
      // "x-hasura-role": "public",
      "Authorization": "",
    }
  };
};

let useLoadProfileInfo = () => {
  let (_, dispatch) = React.useContext(RootContext.context);
  let headers = useHeaders();
  // TODO: this reloads a second hasura client. Rather turn this into a 'fetch' request. OR turn this into a hook directly (not a hook that returns a function).
  let client = Client.useGlobalApolloInstance(headers);
  () => {
    MeQuery.(
      client
      ->ApolloClient.query(~query=(module MeQuery), ())
      ->Js.Promise.then_(
          (result: ApolloClient__ApolloClient.ApolloQueryResult.t(_)) => {
            switch (result) {
            | {data: Some({user: {github: Some(userDetails)}})} =>
              dispatch(LoadDetails(userDetails))
            | _ =>
              %log.error
              "ERROR: user's info not found";
              ();
            };
            ()->async;
          },
          _,
        )
      ->Js.Promise.catch(
          error => {
            [%log.error "ERROR: loading user's info"; ("error", error)];
            ()->async;
          },
          _,
        )
    );
  };
};

let useLogin = () => {
  let (_, dispatch) = React.useContext(RootContext.context);
  authHeader => {
    dispatch(Login(authHeader));
  };
};

let useIsLoggedIn: unit => bool =
  () => {
    let (state, _) = React.useContext(RootContext.context);
    state.isLoggedIn;
  };

let useCurrentUserDetails: unit => option(userDetailsObj) =
  () => {
    let (state, _) = React.useContext(RootContext.context);
    state.userDetails;
  };

let useCurrentUserDetailsWithDefault: unit => userDetailsObj =
  () => {
    useCurrentUserDetails()
    ->Option.mapWithDefault(
        MeQuery.{bio: None, name: None, login: ""},
        info => {
          %log.warn
          "Users data isn't loaded yet. Make sure you only run this hook inside components that are inside the `<Login>` component!";
          info;
        },
      );
  };
