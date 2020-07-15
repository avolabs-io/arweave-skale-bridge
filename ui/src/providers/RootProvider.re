type authHeader = {
  [@bs.as "Authorization"]
  auth: string,
};
type rootContextType = {
  isLoggedIn: bool,
  userId: string,
  userName: string,
  authHeader,
};
type rootActions =
  | LogOut
  | Login(authHeader)
  | LoadDetails(string, string);

let defaultContext = {
  isLoggedIn: false,
  userId: "",
  userName: "",
  authHeader: {
    auth: "",
  },
};

let reducer = (prevState, action) =>
  switch (action) {
  | LogOut => defaultContext
  | Login(authHeader) => {...prevState, isLoggedIn: true, authHeader}
  | LoadDetails(userName, userId) => {...prevState, userName, userId}
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

let useLogin = () => {
  let (_, dispatch) = React.useContext(RootContext.context);
  authHeader => {
    // calls OneGraph.login...
    dispatch(Login(authHeader));
  };
};

let useLoadDetails: (string, string) => unit =
  (userName, userId) => {
    let (_, dispatch) = React.useContext(RootContext.context);
    dispatch(LoadDetails(userName, userId));
  };

let useIsLoggedIn: unit => bool =
  () => {
    let (state, _) = React.useContext(RootContext.context);
    state.isLoggedIn;
  };
