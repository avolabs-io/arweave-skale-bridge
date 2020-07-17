// [@bs.deriving jsConverter]
type logins = string; //[ | `github | `youtube | `facebook];
// type apiAuthHeaders;
type authConstructorArgs = {appId: string};
type t = {
  login: (. logins) => Js.Promise.t(unit),
  logout: (. logins) => Js.Promise.t(unit),
  isLoggedIn: (. logins) => Js.Promise.t(bool),
  authHeaders: (. unit) => RootProvider.authHeader,
};
[@bs.module "onegraph-auth"] [@bs.new]
external initialize: authConstructorArgs => t = "default";

// [@bs.send] external login: (t, logins) => Js.Promise.t(unit) = "login";
// [@bs.send] external isLoggedIn: (t, logins) => Js.Promise.t(bool);

// [@bs.send] external authHeaders: (t, unit) => apiAuthHeaders;
