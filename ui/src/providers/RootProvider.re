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

let loadProfileInfo = dispatch => {
  // MeQuery.(
  //   Client.instance
  //   ->ApolloClient.query(~query=(module MeQuery), ())
  //   // let {me} = onegraphResponse;
  //   // dispatch(LoadDetails(userDataResponse.github.login, "b"))
  //   // let {me} = userDataResponse->Obj.magic;
  //   // let user = userDataResponse.user;
  //   ->Js.Promise.then_(
  //       (result: ApolloClient__ApolloClient.ApolloQueryResult.t(_)) => {
  //         switch (result) {
  //         | {data: Some({user: {github: Some({login, name, bio})}})} =>
  //           ();
  //           dispatch();
  //         | _ =>
  //           %log.error
  //           "ERROR: user's info not found";
  //           ();
  //         };
  //         ()->async;
  //       },
  //       _,
  //     )
  //   ->Js.Promise.catch(
  //       error => {
  //         [%log.error "ERROR: loading user's info"; ("error", error)];
  //         ()->async;
  //       },
  //       _,
  //     )
  // );
  ()
  ->async;
};

let useLogin = () => {
  let (_, dispatch) = React.useContext(RootContext.context);
  authHeader => {
    // calls OneGraph.login...
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

let useHeaders = () => {
  let (state, _) = React.useContext(RootContext.context);
  // state.authHeader;
  {"x-hasura-role": "public"};
};
