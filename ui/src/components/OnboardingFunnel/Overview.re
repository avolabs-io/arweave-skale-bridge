[@react.component]
let make = (~moveToNextStep) => {
  <div className="funnel-step-container overview">
    <h2> "Overview"->React.string </h2>
    <h4> {js| 👉🏽 |js}->React.string "Step 1"->React.string </h4>
    <p> "- Specify Skale Endpoint"->React.string </p>
    <h4> {js| 👉🏽 |js}->React.string "Step 2"->React.string </h4>
    <p> "- Specify Arweave endpoint"->React.string </p>
    <h4> {js| 👉🏽 |js}->React.string "Step 3"->React.string </h4>
    <p> "- Select Data Type"->React.string </p>
    <h4> {js| 👉🏽 |js}->React.string "Step 4"->React.string </h4>
    <p> "- Set frequency"->React.string </p>
    <button onClick=moveToNextStep className="full-width-button">
      "Get Started"->React.string
    </button>
  </div>;
};
