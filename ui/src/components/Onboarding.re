[%raw "require('../styles/css/create-stream.css')"];

type onboardingSteps =
  | Overview
  | SkaleEndpoint
  | SkaleDataTypeToStore
  | Frequency
  | ArweaveEndpoint
  | TopupArweaveWallet
  | OnboardingComplete;

module GeneratingArweaveWallet = {
  [@react.component]
  let make = (~moveToNextStep, ~moveToPrevStep) => {
    <div className="funnel-step-container">
      "step 2"->React.string
      <NavigationButtons moveToNextStep moveToPrevStep />
    </div>;
  };
};

[@react.component]
let make = () => {
  let (onboardingStep, setOnboardingStep) = React.useState(_ => Frequency); //TODO remember to change this back to Overview

  switch (onboardingStep) {
  | Overview =>
    <Overview moveToNextStep={_ => setOnboardingStep(_ => SkaleEndpoint)} />
  | SkaleEndpoint =>
    <SkaleEndpoint
      moveToPrevStep={_ => setOnboardingStep(_ => Overview)}
      moveToNextStep={_ => setOnboardingStep(_ => SkaleDataTypeToStore)}
    />
  | SkaleDataTypeToStore =>
    <SkaleDataTypeToStore
      moveToPrevStep={_ => setOnboardingStep(_ => SkaleEndpoint)}
      moveToNextStep={_ => setOnboardingStep(_ => Frequency)}
    />
  | Frequency =>
    <Frequency
      moveToPrevStep={_ => setOnboardingStep(_ => SkaleDataTypeToStore)}
      moveToNextStep={_ => setOnboardingStep(_ => ArweaveEndpoint)}
    />
  | ArweaveEndpoint =>
    <ArweaveEndpoint
      moveToPrevStep={_ => setOnboardingStep(_ => Frequency)}
      moveToNextStep={_ => setOnboardingStep(_ => TopupArweaveWallet)}
    />
  | TopupArweaveWallet =>
    <div>
      "Create & List arweave public key to load with tokens"->React.string
    </div>
  | OnboardingComplete => <div> "DONE"->React.string </div>
  };
};
