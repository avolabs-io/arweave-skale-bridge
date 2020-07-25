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

type times =
  | TwoHours
  | SixHours
  | TwiceADay
  | Daily
  | Weekly
  | Monthly
  | Custom(int);

let timeToSecond = time =>
  switch (time) {
  | TwoHours => 7200
  | SixHours => 21600
  | TwiceADay => 43200
  | Daily => 86400
  | Weekly => 604800
  | Monthly => 2592000 // 30 days in a month
  | Custom(value) => value
  };

let timeToText = time =>
  switch (time) {
  | TwoHours => "2 Hours"
  | SixHours => "6 Hours"
  | TwiceADay => "Twice a day"
  | Daily => "Daily"
  | Weekly => "Weekly"
  | Monthly => "Monthly"
  | Custom(value) =>
    "Custom setting of " ++ value->string_of_int ++ " seconds"
  };

let secondToTimeSelection = time =>
  switch (time) {
  | 7200 => TwoHours
  | 21600 => SixHours
  | 43200 => TwiceADay
  | 86400 => Daily
  | 604800 => Weekly
  | 2592000 => Monthly
  | value => Custom(value)
  };

[@react.component]
let make =
    (~moveToNextStep, ~moveToPrevStep, ~frequencyInput, ~setFrequencyInput) => {
  let timeOptionsList = [|
    TwoHours,
    SixHours,
    TwiceADay,
    Daily,
    Weekly,
    Monthly,
  |];

  <div className="funnel-step-container frequency radio-box-container">
    <h2> "Frequency"->React.string </h2>
    <h4>
      "Please specify the frequency the data will be backed up"->React.string
    </h4>
    <div>
      <div>
        <ul>
          {timeOptionsList
           ->Belt.Array.map(frequency => {
               let frequencyText = frequency->timeToText;
               let frequencyInt = frequency->timeToSecond;
               let frequencyIntString = frequencyInt->string_of_int;
               <li
                 key=frequencyIntString
                 onClick={_event => {setFrequencyInput(_ => Some(frequency))}}>
                 <input
                   type_="radio"
                   checked={
                     frequencyInt
                     == frequencyInput
                        ->Option.getWithDefault(Custom(-1))
                        ->timeToSecond
                   }
                   id="frequency"
                   name="selector"
                   value=frequencyIntString
                   readOnly=true
                 />
                 <label htmlFor=frequencyText>
                   frequencyText->React.string
                 </label>
                 <div className="check" />
               </li>;
             })
           ->React.array}
        </ul>
      </div>
    </div>
    <NavigationButtons
      moveToNextStep
      moveToPrevStep
      nextDisabled={frequencyInput->Option.isNone}
    />
  </div>;
};
