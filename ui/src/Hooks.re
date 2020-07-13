module PersonsOlderThanQuery = [%graphql
  {|
  query getUserList {
    bridge_user {
      id
    }
  }
|}
];

let age = 40;

[@react.component]
let make = () => {
  <div>
     "something"->React.string </div>;
    // {switch (queryResult) {
    //  | {loading: true, data: None} => <p> "Loading"->React.string </p>
    //  | {loading, data: Some(data), error} =>
    //    <>
    //      <dialog>
    //        {loading ? <p> "Refreshing..."->React.string </p> : React.null}
    //        {switch (error) {
    //         | Some(_) =>
    //           <p>
    //             "The query went wrong, data may be incomplete"->React.string
    //           </p>
    //         | None => React.null
    //         }}
    //      </dialog>
    //      <h4>
    //        {React.string(
    //           "There are "
    //           ++ data.allPersons->Belt.Array.length->string_of_int
    //           ++ " people older than "
    //           ++ age->string_of_int,
    //         )}
    //      </h4>
    //    </>
    //  | {loading: false, data: None} =>
    //    <p> "Error loading data"->React.string </p>
    //  }}
    // );
    //   PersonsOlderThanQuery.makeVariables(~age, ()),
    //   ~errorPolicy=All,
    //   ~fetchPolicy=CacheAndNetwork,
    // PersonsOlderThanQuery.use(
    // let queryResult =
};
