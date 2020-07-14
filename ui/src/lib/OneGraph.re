type t;
[@bs.module "onegraph-auth"] [@bs.new]
external initialize: unit => t = "default";

[@bs.deriving jsConverter]
type logins = [ | `github | `youtube | `facebook];
[@bs.send] external login: logins => Js.Promise.t(unit) = "login";
[@bs.send] external isLoggedIn: logins => Js.Promise.t(bool);
type apiAuthHeaders;
[@bs.send] external authHeaders: unit => apiAuthHeaders;
