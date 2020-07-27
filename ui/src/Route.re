type t =
  | Main
  | Config
  | About
  | CreateBridge
  | Bridge(int)
  | Profile
  | Bridges;

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
  | ["config"] => Config->Some
  | ["about"] => About->Some
  | ["create-bridge"] => CreateBridge->Some
  | ["profile"] => Profile->Some
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
  | Config => "/config"
  | About => "/about"
  | CreateBridge => "/create-bridge"
  | Profile => "/profile"
  | Bridges => "/bridges"
  | Bridge(id) => "/bridge/" ++ id->string_of_int
  };
