/// NOTE: this is temporary code until we have proper authentication in place!
[@bs.val] [@bs.scope "localStorage"] external getItem: string => string;
[@bs.val] [@bs.scope "localStorage"]
external setItem: (string, string) => unit;

let setUser = user => user |> setItem("current-user");
let getUser = () => getItem("current-user");

let isLoggedIn = () => getUser() != "";
