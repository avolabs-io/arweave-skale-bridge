[@react.component]
let make = () => {
  let route = Router.useRouter();
  <div className="app">
    {switch (route) {
     | Some(Config) => <Login> <Config /> </Login>
     | Some(Main) => <Login> <p> "Home page"->React.string </p> </Login>
     | Some(About) => <About />
     | Some(GqlExamplesPage) => <GqlExamples />
     | None => <NotFound />
     }}
  </div>;
};
