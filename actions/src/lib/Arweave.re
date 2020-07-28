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

// const arweaveMainetConfig = {
//   host: "arweave.net", // Hostname or IP address for a Arweave host
//   port: 443, // Port
//   protocol: "https", // Network protocol http or https
//   timeout: 10 * 60 * 1000, // Network request timeouts in milliseconds
//   logging: false,
// };

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
