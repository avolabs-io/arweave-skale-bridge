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
// TODO: quite a few of these values are actually optional...
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

type transactionData = {data: NodeJs.Buffer.t};
[@bs.send]
external createTransaction:
  (t, transactionData, jwk) => Js.Promise.t(transactionResult) =
  "createTransaction";

[@bs.send] [@bs.scope "transactions"]
external transactionsSign: (t, transactionData) => Js.Promise.t(unit) =
  "sign";

type uploader;

[@bs.send] [@bs.scope "transactions"]
external transactionsUpload: (t, transactionData) => Js.Promise.t(uploader) =
  "getUploader";

[@bs.get] [@bs.scope "transactions"]
external isUploadComplete: uploader => bool = "isComplete";

[@bs.send]
external uploadChunk: uploader => Js.Promise.t(unit) = "uploadChunk";

[@bs.send] external pctComplete: uploader => int = "pctComplete";
[@bs.send] external uploadedChunks: uploader => int = "uploadedChunks";
[@bs.send] external totalChunks: uploader => int = "totalChunks";
