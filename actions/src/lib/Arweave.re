type t;
type arweaveProtocol = string;

type initParams = {
  host: string,
  port: int,
  protocol: arweaveProtocol,
  timeout: option(int),
  logging: option(bool),
};

let defaultArweaveParams = {
  host: "127.0.0.1",
  port: 1984,
  protocol: "http",
  timeout: None,
  logging: None,
};

[@bs.module "arweave/node"] external init: initParams => t = "init";

[@decco.encode]
type jwk = {
  kty: string,
  n: string,
  e: string,
  d: string,
  p: string,
  q: string,
  dp: string,
  dq: string,
  qi: string,
};

[@bs.send] [@bs.scope "wallets"]
external generateWallet: (t, unit) => Js.Promise.t(jwk) = "generate";

[@bs.send] [@bs.scope "wallets"]
external getPublicKey: (t, jwk) => Js.Promise.t(string) = "jwkToAddress";

let defaultInstance = init(defaultArweaveParams);
