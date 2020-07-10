[@react.component]
let make = () => {
  let route = Router.useRouter();

  // <NotFound />;
  switch (route) {
  | Some(Main) => <p> "MAIN"->React.string </p>
  | Some(GqlExamplesPage) => <GqlExamples />
  | None => <NotFound />
  };
};
