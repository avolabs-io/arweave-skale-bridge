[@react.component]
let make = (~moveToNextStep, ~moveToPrevStep) => {
  <div className="prev-next-container">
    <button onClick=moveToPrevStep>
      {js|◀|js}->React.string
      " Prev"->React.string
    </button>
    <button onClick=moveToNextStep>
      "Next "->React.string
      {js|►|js}->React.string
    </button>
  </div>;
};
