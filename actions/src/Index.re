open Serbet.Endpoint;
open Globals;

Dotenv.config();
[@bs.val] external port: string = "process.env.PORT";

// TODO: work out why this query errors?
// mutation CreateWallet($privKey: jsonb!, $pubKey: String!, $userId: String!) {
module AddArweaveWallet = [%graphql
  {|
  mutation CreateWallet($privKey: jsonb!, $pubKey: String!, $userId: String!) {
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

        Js.log(jwk);
        let jwkString = Js.Json.stringify(jwk->Obj.magic);
        Js.log(jwkString);

        let%Async pubKey = instance->Arweave.getPublicKey(jwk);

        let escapeString = [%bs.raw
          {|stringToEscape => escape(stringToEscape)|}
        ];
        let jwkStringEscaped = escapeString(jwkString);

        Client.instance
        ->ApolloClient.mutate(
            ~mutation=(module AddArweaveWallet),
            {
              privKey:
                {
                  kty: jwk.kty,
                  n: "Hasura sucks, why does this work but not the real data??",
                  e: "Hasura sucks, why does this work but not the real data??",
                  d: "Hasura sucks, why does this work but not the real data??",
                  p: "Hasura sucks, why does this work but not the real data??",
                  q: "Hasura sucks, why does this work but not the real data??",
                  dp: "Hasura sucks, why does this work but not the real data??",
                  dq: "Hasura sucks, why does this work but not the real data??",
                  qi:
                    "random value to make it accept this"
                    ++ Js.Math.random_int(0, 10000000)->string_of_int,
                }
                ->Arweave.jwk_encode,
              // jwk->Arweave.jwk_encode,
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
