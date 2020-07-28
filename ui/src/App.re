open Router;

[@react.component]
let make = () => {
  let route = useRouter();
  <div className="app">
    <Menu />
    {switch (route) {
     | Some(Main) => <Login> <Home /> </Login>
     | Some(About) => <Login loginProtected=false> <About /> </Login>
     | Some(CreateBridge) => <Login> <Onboarding /> </Login>
     | Some(Bridges) => <Login> <Bridges /> </Login>
     | Some(Bridge(bridgeId)) => <Login> <Bridge bridgeId /> </Login>
     | None => <NotFound />
     }}
  </div>;
};
