Dotenv.config();
[@bs.val] external appId: string = "process.env.REACT_APP_LOGIN_APP_ID";

[@react.component]
let make = () => {
  let authObject = OneGraph.initialize({appId: appId});
  let isLoggedIn = RootProvider.useIsLoggedIn();
  let performLogout = RootProvider.useLogout();

  let onLogout = _ => {
    performLogout(() => authObject.logout(. "github"));
    ();
  };

  let (menuOpen, setMenuOpen) = React.useState(() => false);
  let onClick = _ => {
    setMenuOpen(_ => !menuOpen);
  };
  <div className="menu">
    <button onClick>
      {menuOpen
         ? <img src="/assets/icons/close.svg" />
         : <img src="/assets/icons/hamburger.svg" />}
    </button>
    {menuOpen
       ? <ul>
           <li> <a href="/"> "Home"->React.string </a> </li>
           <li> <a href="/about"> "About"->React.string </a> </li>
           {isLoggedIn
              ? <>
                  <li>
                    <a href="/create-bridge">
                      "Create bridge"->React.string
                    </a>
                  </li>
                  <hr />
                  <li>
                    <span onClick=onLogout>
                      <span className="menu-item-with-icon">
                        <span> "Logout "->React.string </span>
                        <img src="/assets/icons/logout-white.svg" />
                      </span>
                    </span>
                  </li>
                </>
              : <>
                  <hr />
                  <li>
                    <a href="/">
                      <span className="menu-item-with-icon">
                        <span> "Login "->React.string </span>
                        <img src="/assets/icons/github-white.svg" />
                      </span>
                    </a>
                  </li>
                </>}
         </ul>
       : React.null}
  </div>;
};
