[%raw "require('../styles/css/create-stream.css')"];

type onboardingSteps =
  | Overview
  | SkaleEndpoint
  | ArweaveEndpoint
  | GenerateArweaveWallet
  | SkaleDataTypeToStore
  | Frequency
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
  let (onboardingStep, setOnboardingStep) =
    React.useState(_ => ArweaveEndpoint); //TODO remember to change this back to Overview

  switch (onboardingStep) {
  | Overview =>
    <Overview moveToNextStep={_ => setOnboardingStep(_ => SkaleEndpoint)} />
  | SkaleEndpoint =>
    <SkaleEndpoint
      moveToNextStep={_ => setOnboardingStep(_ => SkaleDataTypeToStore)}
      moveToPrevStep={_ => setOnboardingStep(_ => Overview)}
    />
  | SkaleDataTypeToStore =>
    <SkaleDataTypeToStore
      moveToNextStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
      moveToPrevStep={_ => setOnboardingStep(_ => SkaleEndpoint)}
    />
  | Frequency =>
    <Frequency
      moveToNextStep={_ => setOnboardingStep(_ => TopupArweaveWallet)}
      moveToPrevStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
    />
  | GenerateArweaveWallet =>
    <GeneratingArweaveWallet
      moveToNextStep={_ => setOnboardingStep(_ => ArweaveEndpoint)}
      moveToPrevStep={_ => setOnboardingStep(_ => SkaleEndpoint)}
    />
  | ArweaveEndpoint =>
    <ArweaveEndpoint
      moveToNextStep={_ => setOnboardingStep(_ => TopupArweaveWallet)}
      moveToPrevStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
    />

  | TopupArweaveWallet =>
    <div> "List arweave public key to load with tokens"->React.string </div>
  | OnboardingComplete => <div> "DONE"->React.string </div>
  };
};
