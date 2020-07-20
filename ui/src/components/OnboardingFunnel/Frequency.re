[%raw "require('../../styles/css/radio-select.css')"];
open Globals;

module GetFrequenciesQuery = [%graphql
  {|
  query GetFrequenciesQuery {
    frequency_options {
      label
      frequency_in_seconds
    }
  }
|}
];

[@react.component]
let make = (~moveToNextStep, ~moveToPrevStep) => {
  let (_frequencyRadio, setFrequencyRadio) = React.useState(_ => "3600");
  let frequenciesQueryResult =
    GetFrequenciesQuery.use(
      ~fetchPolicy=CacheAndNetwork,
      ~errorPolicy=All,
      GetFrequenciesQuery.makeVariables(),
    );

  <div className="funnel-step-container frequency radio-box-container">
    <h2> "Frequency"->React.string </h2>
    <h4>
      "Please specify the frequency the data will be backed up"->React.string
    </h4>
    <div>
      {switch (frequenciesQueryResult) {
       | {loading: true, data: None} => <p> "Loading"->React.string </p>
       | {loading, data: Some(data), error} =>
         <>
           <dialog>
             {loading ? <p> "Refreshing..."->React.string </p> : React.null}
             {switch (error) {
              | Some(_) =>
                <p>
                  "The query went wrong, data may be incomplete"->React.string
                </p>
              | None => React.null
              }}
           </dialog>
           <div>
             <ul>
               {data.frequency_options
                ->Belt.Array.map(frequency =>
                    <li>
                      <input
                        type_="radio"
                        id={frequency.label}
                        name="selector"
                        value={
                          frequency.frequency_in_seconds
                          ->Option.getWithDefault(0)
                          ->string_of_int
                        }
                        onChange={event => {
                          let value = ReactEvent.Form.target(event)##value;
                          setFrequencyRadio(_ => value);
                        }}
                      />
                      <label htmlFor={frequency.label}>
                        frequency.label->React.string
                      </label>
                      <div className="check" />
                    </li>
                  )
                ->React.array}
             </ul>
           </div>
         </>
       | {loading: false, data: None} =>
         <p> "Error loading data"->React.string </p>
       }}
    </div>
    //  )}
    //    </li>
    //      <div className="check" />
    //      </label>
    //        {frequency.frequency}->React.string
    //      <label htmlFor={frequency.frequency}>
    //      />
    //        // onChange={setFrequencyRadio(_ => "hourly")}
    //        value={frequency.frequency}
    //        name="selector"
    //        id={frequency.frequency}
    //        type_="radio"
    //      <input
    //    <li>
    // {arrayData->Belt.Array.map(frequency =>
    <NavigationButtons moveToNextStep moveToPrevStep />
  </div>;
};
