[%raw "require('../../styles/css/radio-select.css')"];

[@react.component]
let make = (~moveToNextStep, ~moveToPrevStep) => {
  let (_frequencyRadio, _setFrequencyRadio) = React.useState(_ => "hourly");

  <div className="funnel-step-container frequency" >
    <h2>
      "Please specify the frequency the data will be backed up"->React.string
    </h2>
    <div>
      <ul>
        <li>
          <input
            type_="radio"
            id="h-option"
            name="selector"
            value="hourly"
            // onChange={setFrequencyRadio(_ => "hourly")}
          />
          <label htmlFor="h-option"> "Hourly"->React.string </label>
          <div className="check" />
        </li>
        <li>
          <input
            type_="radio"
            id="d-option"
            name="selector"
            value="daily"
            // onChange={setFrequencyRadio(_ => "daily")}
          />
          <label htmlFor="d-option"> "Daily"->React.string </label>
          <div className="check"> <div className="inside" /> </div>
        </li>
        <li>
          <input
            type_="radio"
            id="w-option"
            name="selector"
            value="weekly"
            // onChange={setFrequencyRadio(_ => "weekly")}
          />
          <label htmlFor="w-option"> "Weekly"->React.string </label>
          <div className="check"> <div className="inside" /> </div>
        </li>
      </ul>
    </div>
    <NavigationButtons moveToNextStep moveToPrevStep />
  </div>;
};
