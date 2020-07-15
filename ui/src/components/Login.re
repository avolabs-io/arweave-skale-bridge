open Globals;

Dotenv.config();
[@bs.val] external appId: string = "process.env.REACT_APP_LOGIN_APP_ID";

module MeQuery = [%graphql
  {|
  query MeQuery {
     me {
       github {
         login
         bio
       }
     }
   }
   |}
];

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

let getProfileInfo = () => {
  Client.instance
  ->ApolloClient.query(~query=(module MeQuery), ())
  ->Js.Promise.then_(
      (result: ApolloClient__ApolloClient.ApolloQueryResult.t(_)) =>
        switch (result) {
        | {data} => Js.Promise.resolve(Js.log2("Data: ", data))
        | _ => Js.Exn.raiseError("Error: no people!")
        },
      _,
    )
  ->Js.Promise.catch(
      eeyore => Js.Promise.resolve(Js.log2("Error: ", eeyore)),
      _,
    )
  ->ignore;
  ();
};

[@react.component]
let make = (~children) => {
  let authObject = OneGraph.initialize({appId: appId});
  let (isLoggedIn, setLoggedIn) = React.useState(_ => false);
  let handleLoginStatusChange = loginStatus => {
    if (loginStatus) {
      Js.log(authObject.authHeaders(.));
      getProfileInfo();
    } else {
      ();
    };
    setLoggedIn(_ => loginStatus);
  };
  React.useEffect0(() => {
    Js.Promise.then_(
      loginStatus => {handleLoginStatusChange(loginStatus)->async},
      authObject.isLoggedIn(. "github"),
    )
    ->ignore;
    None;
  });
  let onClick = _ => {
    Js.Promise.then_(
      _ => {
        Js.Promise.then_(
          loginStatus => {handleLoginStatusChange(loginStatus)->async},
          authObject.isLoggedIn(. "github"),
        )
        ->ignore;

        ()->async;
      },
      authObject.login(. "github"),
    )
    ->ignore;
    ();
  };
  /*
     {
     me {
       github {
         login
         bio
       }
     }
   }
      */
  <div>
    <button onClick> "Login with Github"->React.string </button>
    {isLoggedIn
       ? <p> "logged in"->React.string </p>
       : <p> "Not logged in"->React.string </p>}
    children
  </div>;
};
