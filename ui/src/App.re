module LoginScreen = {
  [@bs.module "./deprecated/login.js"] [@react.component]
  external make: unit => React.element = "default";
};

[@react.component]
let make = () => {
  let route = Router.useRouter();

  // <NotFound />;
  switch (route) {
  // | Some(Main) => <p> "MAIN"->React.string </p>
  | Some(Config) => <Config />
  | Some(Main) => <LoginScreen />
  // | Some(Main) => <p> "MAIN"->React.string </p>
  | Some(GqlExamplesPage) => <GqlExamples />
  | None => <NotFound />
  };
};
