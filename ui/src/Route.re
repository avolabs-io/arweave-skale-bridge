type t =
  | Main
  | Config
  | GqlExamplesPage;

let fromUrl = (url: ReasonReactRouter.url) =>
  switch (url.path) {
  | [] => Main->Some
  | ["config"] => Config->Some
  | ["gql-examples-page"] => GqlExamplesPage->Some
  | _ => None // 404
  };
