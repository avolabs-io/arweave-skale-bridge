[%raw "require('../../styles/css/radio-select.css')"];

[@react.component]
let make = (~moveToNextStep, ~moveToPrevStep) => {
  <div className="funnel-step-container">
    <h2>
      "Please specify the frequency the data should be backed up"->React.string
    </h2>
    <div>
      <ul>
        <li>
          <input type_="radio" id="h-option" name="selector" />
          <label htmlFor="h-option"> "Hourly"->React.string </label>
          <div className="check" />
        </li>
        <li>
          <input type_="radio" id="d-option" name="selector" />
          <label htmlFor="d-option"> "Daily"->React.string </label>
          <div className="check"> <div className="inside" /> </div>
        </li>
        <li>
          <input type_="radio" id="w-option" name="selector" />
          <label htmlFor="w-option"> "Weekly"->React.string </label>
          <div className="check"> <div className="inside" /> </div>
        </li>
      </ul>
    </div>
    <NavigationButtons moveToNextStep moveToPrevStep />
  </div>;
};
