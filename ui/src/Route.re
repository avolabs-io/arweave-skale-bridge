type t =
  | Main
  | GqlExamplesPage;

let fromUrl = (url: ReasonReactRouter.url) =>
  switch (url.path) {
  | [] => Main->Some
  | ["gql-examples-page"] => GqlExamplesPage->Some
  | _ => None // 404
  };
