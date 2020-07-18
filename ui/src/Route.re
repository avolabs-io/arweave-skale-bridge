type t =
  | Main
  | Config
  | About
  | CreateBridge
  | Profile
  | Dashboard
  | GqlExamplesPage;

let fromUrl = (url: ReasonReactRouter.url) =>
  switch (url.path) {
  | [] => Main->Some
  | ["config"] => Config->Some
  | ["about"] => About->Some
  | ["create-bridge"] => CreateBridge->Some
  | ["profile"] => Profile->Some
  | ["dashboard"] => Dashboard->Some
  | ["gql-examples-page"] => GqlExamplesPage->Some
  | _ => None // 404
  };
