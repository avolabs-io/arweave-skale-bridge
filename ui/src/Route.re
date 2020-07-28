type t =
  | Main
  | About
  | CreateBridge
  | Bridges
  | Bridge(int);

type onboardingSteps =
  | Overview
  | SkaleEndpoint
  | SkaleDataTypeToStore
  | Frequency
  | ArweaveEndpoint
  | TopupArweaveWallet
  | OnboardingComplete;

let fromUrl = (url: ReasonReactRouter.url) =>
  switch (url.path) {
  | [] => Main->Some  
  | ["about"] => About->Some
  | ["create-bridge"] => CreateBridge->Some
  | ["bridges"] => Bridges->Some
  | ["bridge", id] =>
    switch (id->int_of_string_opt) {
    | Some(bridgeId) => Bridge(bridgeId)->Some
    | None => None
    }
  | _ => None // 404
  };

let toString = route =>
  switch (route) {
  | Main => "/"
  | About => "/about"
  | CreateBridge => "/create-bridge"
  | Bridges => "/bridges"
  | Bridge(id) => "/bridge/" ++ id->string_of_int
  };
