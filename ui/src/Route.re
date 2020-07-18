type t =
  | Main
  | Config
  | CreateBridge
  | About
  | GqlExamplesPage;

let fromUrl = (url: ReasonReactRouter.url) =>
  switch (url.path) {
  | [] => Main->Some
  | ["config"] => Config->Some
  | ["about"] => About->Some
  | ["create-bridge"] => CreateBridge->Some
  | ["gql-examples-page"] => GqlExamplesPage->Some
  | _ => None // 404
  };
