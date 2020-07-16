open Globals;

Dotenv.config();
[@bs.val] external appId: string = "process.env.REACT_APP_LOGIN_APP_ID";

[@react.component]
let make = (~children) => {
  let authObject = OneGraph.initialize({appId: appId});
  let isLoggedIn = RootProvider.useIsLoggedIn();
  let performLogin = RootProvider.useLogin();
  let (isLoadingAuth, setIsLoadingAuth) = React.useState(_ => true);

  React.useEffect0(() => {
    Js.Promise.then_(
      loginStatus => {
        setIsLoadingAuth(_ => false);
        (
          if (loginStatus) {
            performLogin(authObject.authHeaders(.));
          } else {
            ();
          }
        )
        ->async;
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
          loginStatus =>
            (
              if (loginStatus) {
                performLogin(authObject.authHeaders(.));
              } else {
                ();
              }
            )
            ->async,
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
    {isLoggedIn
       ? children
       : {
         isLoadingAuth
           ? <h1>
               "DENHAM - I leave it up to you to put a proper login loader of your choice here :)"
               ->React.string
             </h1>
           : <React.Fragment>
               <div> <img src="./assets/skale-arweave.svg" /> </div>
               <button onClick className="login-button">
                 "LOGIN WITH GITHUB"->React.string
                 <img src="./assets/github.svg" className="gh-icon" />
               </button>
             </React.Fragment>;
       }}
  </div>;
};
