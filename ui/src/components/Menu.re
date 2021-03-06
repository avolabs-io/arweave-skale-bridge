open Router;

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
    <div className="menu-inner">
      <button onClick>
        {menuOpen
           ? <img src="/assets/icons/close.svg" />
           : <img src="/assets/icons/hamburger.svg" />}
      </button>
      {menuOpen
         ? <ul>
             <li onClick> <Link route=Main> "Home"->React.string </Link> </li>
             <li onClick>
               <Link route=About> "About"->React.string </Link>
             </li>
             {isLoggedIn
                ? <>
                    <li onClick>
                      <Link route=Bridges> "Bridges"->React.string </Link>
                    </li>
                    <li onClick>
                      <Link route=CreateBridge>
                        "Create bridge"->React.string
                      </Link>
                    </li>
                    <hr />
                    <li onClick>
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
                    <li onClick>
                      <Link route=Main>
                        <span className="menu-item-with-icon">
                          <span> "Login "->React.string </span>
                          <img src="/assets/icons/github-white.svg" />
                        </span>
                      </Link>
                    </li>
                  </>}
           </ul>
         : React.null}
    </div>
  </div>;
};
