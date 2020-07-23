open Serbet.Endpoint;
open Globals;

Dotenv.config();
[@bs.val] external port: option(string) = "process.env.PORT";
[@bs.val]
external processLoopTime: option(string) =
  "process.env.PROCESS_LOOP_INTERVAL";

let optStringToInt = (optStringInt, default) =>
  optStringInt->Option.flatMap(intString => intString->int_of_string_opt)
  |||| default;

Scheduler.startScheduler(
  processLoopTime->optStringToInt(12000) /*use a default of 2 min*/,
);

module AddArweaveWallet = [%graphql
  {|
  mutation CreateWallet($pubKey: String!, $userId: String!, $privKey: json!) {
    insert_arweave_key_one(object: {priv_key: $privKey, pub_key: $pubKey, user_id: $userId}) {
      user_id
    }
  }
|}
];

module Arweave = {
  [@decco.decode]
  type userIdObj = {userId: string};
  [@decco.decode]
  type body_in = {input: userIdObj};
  [@decco.encode]
  type body_out = {address: string};
  let createArweaveWallet =
    Serbet.jsonEndpoint({
      verb: POST,
      path: "/createArweaveWallet",
      body_in_decode,
      body_out_encode,
      handler: (body, _req) => {
        let instance = Arweave.defaultInstance;
        let%Async jwk = instance->Arweave.generateWallet();
        let%Async pubKey = instance->Arweave.getPublicKey(jwk);

        Client.instance
        ->ApolloClient.mutate(
            ~mutation=(module AddArweaveWallet),
            {
              privKey: jwk->Arweave.jwk_encode,
              pubKey,
              userId: body.input.userId,
            },
          )
        ->mapAsync(result =>
            switch (result) {
            | {data: Some(_), errors: None} => {address: pubKey}
            | {errors: _} => {address: "ERROR saving public key"}
            }
          )
        ->catchAsync(error => {
            Js.log2("error setting the public key", error);

            {address: "ERROR"}->async;
          });
      },
    });
};

let app =
  Serbet.application(
    ~port=port->optStringToInt(9898),
    [Arweave.createArweaveWallet],
  );
