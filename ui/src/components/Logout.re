Dotenv.config();
[@bs.val] external appId: string = "process.env.REACT_APP_LOGIN_APP_ID";

[@react.component]
let make = () => {
  let authObject = OneGraph.initialize({appId: appId});
  let isLoggedIn = RootProvider.useIsLoggedIn();
  let performLogout = RootProvider.useLogout();

  let onClick = _ => {
    performLogout(() => authObject.logout(. "github"));
    ();
  };

  if (isLoggedIn) {
    <button onClick className="logout">
      <img src="./assets/logout.svg" />
    </button>;
  } else {
    <span />;
  };
};
