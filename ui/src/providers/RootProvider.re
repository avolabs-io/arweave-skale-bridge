type rootContextType = {
  isLoggedIn: bool,
  userId: string,
  userName: string,
  authHeader: string,
};

let defaultContext = {
  isLoggedIn: false,
  userId: "",
  userName: "",
  authHeader: "",
};

let rootContext = React.createContext(defaultContext);
module RootContext = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(rootContext);
};

[@react.component]
let make = (~children) => {
  <RootContext value=defaultContext> children </RootContext>;
};
