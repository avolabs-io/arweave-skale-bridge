open Serbet.Endpoint;
open Globals;

Dotenv.config();
[@bs.val] external port: string = "process.env.PORT";

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
  // TODO: add better security to this endpoint.
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
        ->Js.Promise.then_(_result => {address: pubKey}->async, _)
        ->Js.Promise.catch(
            error => {
              Js.log2("error", error);

              {address: "ERROR"}->async;
            },
            _,
          );
      },
    });
};

let app =
  Serbet.application(
    ~port=port->int_of_string_opt |||| 9898,
    [Arweave.createArweaveWallet],
  );
