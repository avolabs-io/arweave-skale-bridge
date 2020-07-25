[@react.component]
let make = (~moveToNextStep, ~moveToPrevStep, ~nextDisabled) => {
  <div className="prev-next-container">
    <button onClick={_ => moveToPrevStep()}>
      {js|◀|js}->React.string
      " Prev"->React.string
    </button>
    <button
      onClick={_ => moveToNextStep()}
      disabled=nextDisabled
      className={nextDisabled ? Css.(style([opacity(0.6)])) : ""}>
      "Next "->React.string
      {js|►|js}->React.string
    </button>
  </div>;
};
