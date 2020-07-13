module LoginScreen = {
  [@bs.module "./login.js"] [@react.component]
  external make: unit => React.element = "default";
};

[@react.component]
let make = () => {
  let route = Router.useRouter();

  // <NotFound />;
  switch (route) {
  | Some(Main) => <LoginScreen />
  // | Some(Main) => <p> "MAIN"->React.string </p>
  | Some(GqlExamplesPage) => <p> "MAIN"->React.string </p>
  // | Some(GqlExamplesPage) => <GqlExamples />
  | None => <NotFound />
  };
};
