module GetSkaleDataTypesQuery = [%graphql
  {|
  query DataTypesQuery {
    skale_data_type_enum{
      data_type
    }
  }
|}
];

module DataTypeSelect = {
  [@react.component]
  let make = () => {
    let usersIdDetails = RootProvider.useCurrentUserDetailsWithDefault();
    let (selectedSkaleDataType, setSelectedSkaleDataType) =
      React.useState(() => "");

    let skaleDataTypeQueryResult =
      GetSkaleDataTypesQuery.use(
        ~fetchPolicy=CacheAndNetwork,
        ~errorPolicy=All,
        GetSkaleDataTypesQuery.makeVariables(),
      );

    <div>
      <div>
        {switch (skaleDataTypeQueryResult) {
         | {loading: true, data: None} => <p> "Loading"->React.string </p>
         | {loading, data: Some(data), error} =>
           <>
             <dialog>
               {loading ? <p> "Refreshing..."->React.string </p> : React.null}
               {switch (error) {
                | Some(_) =>
                  <p>
                    "The query went wrong, data may be incomplete"
                    ->React.string
                  </p>
                | None => React.null
                }}
             </dialog>
             // TODO: this may be more flexible than a normal html select: https://github.com/ahrefs/bs-react-select
             <select
               name="skaleDataType"
               id="skaleDataType"
               value=selectedSkaleDataType
               onChange={event => {
                 let value = ReactEvent.Form.target(event)##value;
                 setSelectedSkaleDataType(_ => value);
               }}>
               {data.skale_data_type_enum
                ->Belt.Array.map(dataType =>
                    <option value={dataType.data_type}>
                      dataType.data_type->React.string
                    </option>
                  )
                ->React.array}
             </select>
           </>
         | {loading: false, data: None} =>
           <p> "Error loading data"->React.string </p>
         }}
      </div>
    </div>;
  };
};

[@react.component]
let make = (~moveToNextStep, ~moveToPrevStep) => {
  let (_frequencyRadio, _setFrequencyRadio) = React.useState(_ => "hourly");

  <div className="funnel-step-container">
    <h2> "Skale Data"->React.string </h2>
    <h4>
      "Please select the type of data you would like to backup"->React.string
    </h4>
    <div> <DataTypeSelect /> </div>
    <NavigationButtons moveToNextStep moveToPrevStep />
  </div>;
};
