type t =
  | Main
  | Config
  | About
  | GqlExamplesPage;

let fromUrl = (url: ReasonReactRouter.url) =>
  switch (url.path) {
  | [] => Main->Some
  | ["config"] => Config->Some
  | ["about"] => About->Some
  | ["gql-examples-page"] => GqlExamplesPage->Some
  | _ => None // 404
  };
