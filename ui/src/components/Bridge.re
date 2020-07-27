module BridgeSyncsQuery = [%graphql
  {|
query Syncs($bridgeId: Int!) {
  bridge_sync(where: {bridge_id: {_eq: $bridgeId}}, limit: 30, order_by: [{index: desc}]) {
    id
    Info
    end_time
    index
    start_time
    status
  }
}
|}
];

[@react.component]
let make = (~bridgeId) => {
  <div>
    {("Info about the bridge #" ++ bridgeId->string_of_int)->React.string}
  </div>;
};
