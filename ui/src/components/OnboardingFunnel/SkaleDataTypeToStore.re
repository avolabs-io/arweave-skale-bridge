open Globals;

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
  let make = (~setSkaleDataType, ~skaleDataTypeInput, ~moveToNextStep) => {
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
           <form onSubmit={_ => Js.log("SUBMITTING THE FORM")}>
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
               value={skaleDataTypeInput->Option.getWithDefault("select")}
               onChange={event => {
                 let value = ReactEvent.Form.target(event)##value;
                 if (value != "select") {
                   setSkaleDataType(_ => Some(value));
                   moveToNextStep();
                 } else {
                   ();
                 };
               }}>
               <option
                 value="select"
                 disabled=true
                 className=Css.(style([backgroundColor(blue)]))>
                 "Select an option"->React.string
               </option>
               {data.skale_data_type_enum
                ->Belt.Array.map(dataType =>
                    <option value={dataType.data_type}>
                      dataType.data_type->React.string
                    </option>
                  )
                ->React.array}
             </select>
           </form>
         | {loading: false, data: None} =>
           <p> "Error loading data"->React.string </p>
         }}
      </div>
    </div>;
  };
};

[@react.component]
let make =
    (~moveToNextStep, ~moveToPrevStep, ~setSkaleDataType, ~skaleDataTypeInput) => {
  <div className="funnel-step-container">
    <h2> "Skale Data"->React.string </h2>
    <h4>
      "Please select the type of data you would like to backup"->React.string
    </h4>
    <div>
      <DataTypeSelect setSkaleDataType skaleDataTypeInput moveToNextStep />
    </div>
    <NavigationButtons
      moveToNextStep
      moveToPrevStep
      nextDisabled={skaleDataTypeInput->Option.isNone}
    />
  </div>;
};
