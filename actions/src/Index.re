open Serbet.Endpoint;
open Globals;

Dotenv.config();
[@bs.val] external port: string = "process.env.PORT";

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
        ()->async;
      },
    });
};

let app =
  Serbet.application(
    ~port=port->int_of_string_opt |||| 9898,
    [StartCron.endpoint],
  );
