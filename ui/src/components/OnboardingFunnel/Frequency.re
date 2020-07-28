[%raw "require('../../styles/css/radio-select.css')"];
open Globals;
open OnboardingHelpers;

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
  | TwiceADay
  | Daily
  | Weekly
  | Monthly
  | Custom(int);

let timeToSecond = time =>
  switch (time) {
  | TwiceADay => 43200
  | Daily => 86400
  | Weekly => 604800
  | Monthly => 2592000 // 30 days in a month
  | Custom(value) => value
  };

let timeToText = time =>
  switch (time) {
  | TwiceADay => "Twice a day"
  | Daily => "Daily"
  | Weekly => "Weekly"
  | Monthly => "Monthly"
  | Custom(value) => value->string_of_int ++ " seconds"
  };

let secondToTimeSelection = time =>
  switch (time) {
  | 43200 => TwiceADay
  | 86400 => Daily
  | 604800 => Weekly
  | 2592000 => Monthly
  | value => Custom(value)
  };

let secondsToText = time => time->secondToTimeSelection->timeToText;

[@react.component]
let make =
    (~moveToNextStep, ~moveToPrevStep, ~frequencyInput, ~setFrequencyInput) => {
  let timeOptionsList = [|TwiceADay, Daily, Weekly, Monthly|];

  <div className="funnel-step-container frequency radio-box-container">
    <HiddenAutoFocusButton
      action={_ => {
        let idToSet = frequencyInput->Option.getWithDefault(Daily);
        setFrequencyInput(_ => Some(idToSet));
        Js.Global.setTimeout(moveToNextStep, 500)->ignore;
      }}
    />
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
               let checked = {
                 frequencyInt
                 == frequencyInput
                    ->Option.getWithDefault(Custom(-1))
                    ->timeToSecond;
               };
               <li
                 key=frequencyIntString
                 onClick={_event => {
                   setFrequencyInput(_ => Some(frequency));
                   Js.Global.setTimeout(moveToNextStep, 500)->ignore;
                 }}>
                 <input
                   type_="radio"
                   checked
                   id="frequency"
                   name="selector"
                   value=frequencyIntString
                   readOnly=true
                 />
                 <label
                   htmlFor=frequencyText
                   className={checked ? selectedItemAnimation : ""}>
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
