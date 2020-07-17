module AuthenticatedRoute = {
  [@react.component]
  let make = (~children) => <Login> children </Login>;
};

[@react.component]
let make = () => {
  let route = Router.useRouter();
  <div className="app">
    <Logout />
    {switch (route) {
     | Some(Config) => <AuthenticatedRoute> <Config /> </AuthenticatedRoute>
     | Some(Main) =>
       <AuthenticatedRoute>
         <p> "Home page"->React.string </p>
       </AuthenticatedRoute>
     | Some(About) => <About />
     | Some(GqlExamplesPage) => <GqlExamples />
     | None => <NotFound />
     }}
  </div>;
};
