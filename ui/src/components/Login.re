open Globals;

Dotenv.config();
[@bs.val] external appId: string = "process.env.REACT_APP_LOGIN_APP_ID";

module MeQuery = [%graphql
  {|
  query MeQuery {
     me {
       github {
         login
         bio
       }
     }
   }
   |}
];

let getProfileInfo = () => {
  Client.instance
  ->ApolloClient.query(~query=(module MeQuery), ())
  ->Js.Promise.then_(
      (result: ApolloClient__ApolloClient.ApolloQueryResult.t(_)) =>
        switch (result) {
        | {data} => Js.Promise.resolve(Js.log2("Data: ", data))
        | _ => Js.Exn.raiseError("Error: no people!")
        },
      _,
    )
  ->Js.Promise.catch(
      eeyore => Js.Promise.resolve(Js.log2("Error: ", eeyore)),
      _,
    )
  ->ignore;
  ();
};

[@react.component]
let make = (~children) => {
  let authObject = OneGraph.initialize({appId: appId});
  let isLoggedIn = RootProvider.useIsLoggedIn();

  React.useEffect0(() => {
    Js.Promise.then_(
      loginStatus => {
        (loginStatus ? RootProvider.useLogin(authObject.authHeaders(.)) : ())
        ->async
      },
      authObject.isLoggedIn(. "github"),
    )
    ->ignore;
    None;
  });
  // TODO: move more of this logic to the RootProvider.
  let onClick = _ => {
    Js.Promise.then_(
      _ => {
        Js.Promise.then_(
          loginStatus => {
            (
              loginStatus
                ? RootProvider.useLogin(authObject.authHeaders(.)) : ()
            )
            ->async
          },
          authObject.isLoggedIn(. "github"),
        )
        ->ignore;

        ()->async;
      },
      authObject.login(. "github"),
    )
    ->ignore;
    ();
  };
  <div className="login">
    <div> <img /> </div>
    <button onClick className="login-button">
      "LOGIN WITH GITHUB"->React.string
      <img src="./assets/github.svg" className="gh-icon" />
    </button>
    <code data="debug" className="code-block">
      {isLoggedIn
         ? <p> "logged in"->React.string </p>
         : <p> "Not logged in"->React.string </p>}
      children
    </code>
  </div>;
};
