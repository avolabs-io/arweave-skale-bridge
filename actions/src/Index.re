open Serbet.Endpoint;
open Globals;

Dotenv.config();
[@bs.val] external port: string = "process.env.PORT";

// let apolloClient = Client.instance;

module PeopleQuery = [%graphql
  {|
  query PeopleQuery {
    people: allPersons {
      id
      name
    }
  }
|}
];

module StartCron = {
  [@decco.decode]
  type body_in = unit;
  [@decco.encode]
  type body_out = unit;

  let endpoint =
    Serbet.jsonEndpoint({
      verb: POST,
      path: "/startWatcher",
      body_in_decode,
      body_out_encode,
      handler: (_body, _req) => {
        Client.instance
        ->ApolloClient.query(~query=(module PeopleQuery), ())
        ->Js.Promise.then_(
            (result: ApolloClient__ApolloClient.ApolloQueryResult.t(_)) =>
              switch (result) {
              | {data: Some({PeopleQuery.people})} =>
                Js.Promise.resolve(Js.log2("Data: ", people))
              | _ => Js.Exn.raiseError("Error: no people!")
              },
            _,
          )
        ->Js.Promise.catch(
            eeyore => Js.Promise.resolve(Js.log2("Error: ", eeyore)),
            _,
          );
      },
    });

  let testEndpoint =
    Serbet.jsonEndpoint({
      verb: POST,
      path: "/test-function",
      body_in_decode,
      body_out_encode,
      handler: (_body, _req) => {
        Js.log("logging something");
        let%Async result =
          Client.instance->ApolloClient.query(
            ~query=(module PeopleQuery),
            (),
          );
        switch (result) {
        | {data: Some({PeopleQuery.people})} =>
          Js.Promise.resolve(Js.log2("Data: ", people))
        | _ => Js.Exn.raiseError("Error: no people!")
        };
      },
    });
};

let app =
  Serbet.application(
    ~port=port->int_of_string_opt |||| 9898,
    [StartCron.endpoint, StartCron.testEndpoint],
  );
