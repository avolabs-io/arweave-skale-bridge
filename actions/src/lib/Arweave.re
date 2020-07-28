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
  host: "arweave.net",
  port: 443,
  protocol: "https",
  timeout: Some(10 * 60 * 1000),
  logging: Some(false),
};

type transactionTag = {
  name: string,
  value: string,
};

// [@decco.encode]
type transactionResult = {
  format: int,
  id: string,
  last_tx: string,
  owner: string,
  tags: array(transactionTag),
  target: string,
  quantity: string,
  data_size: string,
  data_root: string,
  reward: string,
  signature: string,
};

[@bs.module "arweave/node"] external init: initParams => t = "init";

[@decco]
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
