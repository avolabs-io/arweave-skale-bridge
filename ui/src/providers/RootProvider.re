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
let defaultContext = {
  isLoggedIn: false,
  userId: "",
  userName: "",
  authHeader: {
    auth: "",
  },
};

type rootActions =
  | LogOut
  | Login(authHeader)
  | LoadDetails(string, string);

let reducer = (prevState, action) =>
  switch (action) {
  | LogOut => defaultContext
  | Login(authHeader) => {...prevState, isLoggedIn: true, authHeader}
  | LoadDetails(userName, userId) => {...prevState, userName, userId}
  };

let rootContext = React.createContext((defaultContext, _ => ()));

module RootContext = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(rootContext);
};

[@react.component]
let make = (~children) => {
  let (rootState, dispatch) = React.useReducer(reducer, defaultContext);

  <RootContext value=(rootState, dispatch)> children </RootContext>;
};
