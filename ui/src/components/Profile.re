open Globals;

module GetUserArweaveWalletQuery = [%graphql
  {|
  query EndpointQuery ($userId: String!) {
    arweave_key(where: {user_id: {_eq: $userId}}) {
      pub_key
      user_id
    }
  }
|}
];

[@react.component]
let make = () => {
  let loadUserData: RootProvider.MeQuery.t_user_github =
    RootProvider.useCurrentUserDetailsWithDefault();

  let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
  let arweaveWalletQueryResult =
    GetUserArweaveWalletQuery.use(
      ~fetchPolicy=CacheAndNetwork,
      ~errorPolicy=All,
      GetUserArweaveWalletQuery.makeVariables(
        ~userId=usersIdDetails.login,
        (),
      ),
    );

  <div id="profile">
    <h1>
      "Profile "->React.string
      <img src="/assets/icons/profile.svg" className="profile-menu" />
    </h1>
    <h3>
      {loadUserData.name
       ->Option.getWithDefault(loadUserData.login)
       ->React.string}
    </h3>
    {switch (arweaveWalletQueryResult) {
     | {loading: true, data: None} => React.null
     | {loading, data: Some(data), error} =>
       <>
         {loading
            ? <p> "Loading your current data."->React.string </p> : React.null}
         {switch (error) {
          | Some(_) =>
            <p>
              "Unexpected error, please check your connection."->React.string
            </p>
          | None => React.null
          }}
         {switch (data.arweave_key) {
          | [||] => <span />
          | [|{pub_key}|]
          | [|{pub_key}, _|] =>
            <div>
              <p> "Arweave wallet: "->React.string pub_key->React.string </p>
              <p> "Balance: "->React.string "AR"->React.string </p>
            </div>
          }}
         // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
       </>
     | {loading: false, data: None} =>
       <p> "Error loading existing endpoints."->React.string </p>
     }}
  </div>;
};
