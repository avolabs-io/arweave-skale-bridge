module AuthenticatedRoute = {
  [@react.component]
  let make = (~children) => <Login> children </Login>;
};

[@react.component]
let make = () => {
  let route = Router.useRouter();
  <div className="app">
    <Menu />
    // <Logout />
    {switch (route) {
     | Some(Main) =>
       <AuthenticatedRoute>
         <p> "Home page"->React.string </p>
         <a href="/create-bridge"> "Create Bridge"->React.string </a>
       </AuthenticatedRoute>
     | Some(CreateBridge) =>
       <AuthenticatedRoute> <Onboarding /> </AuthenticatedRoute>
     | Some(Config) => <AuthenticatedRoute> <Config /> </AuthenticatedRoute>
     | Some(About) => <About />
     | Some(GqlExamplesPage) => <GqlExamples />
     | None => <NotFound />
     }}
  </div>;
};
