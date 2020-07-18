open Globals;

[@react.component]
let make = () => {
  let loadUserData: RootProvider.MeQuery.t_user_github =
    RootProvider.useCurrentUserDetailsWithDefault();

  <div id="profile">
    <h1> "Profile"->React.string </h1>
    <h3>
      {loadUserData.name
       ->Option.getWithDefault(loadUserData.login)
       ->React.string}
    </h3>
  </div>;
};
