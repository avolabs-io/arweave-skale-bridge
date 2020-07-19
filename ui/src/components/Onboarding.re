[%raw "require('../styles/css/create-stream.css')"];

type onboardingSteps =
  | Overview
  | SkaleEndpoint
  | ArweaveEndpoint
  | GenerateArweaveWallet
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

module CreateArweaveEndpoint = {
  [@react.component]
  let make = (~moveToNextStep, ~moveToPrevStep) => {
    <div className="funnel-step-container">
      "step 3"->React.string
      <NavigationButtons moveToNextStep moveToPrevStep />
    </div>;
  };
};
[@react.component]
let make = () => {
  let (onboardingStep, setOnboardingStep) = React.useState(_ => Overview); //TODO remember to change this back to Overview

  switch (onboardingStep) {
  | Overview =>
    <Overview moveToNextStep={_ => setOnboardingStep(_ => SkaleEndpoint)} />
  | SkaleEndpoint =>
    <SkaleEndpoint
      moveToNextStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
      moveToPrevStep={_ => setOnboardingStep(_ => Overview)}
    />
  | GenerateArweaveWallet =>
    <GeneratingArweaveWallet
      moveToNextStep={_ => setOnboardingStep(_ => ArweaveEndpoint)}
      moveToPrevStep={_ => setOnboardingStep(_ => SkaleEndpoint)}
    />
  | ArweaveEndpoint =>
    <CreateArweaveEndpoint
      moveToNextStep={_ => setOnboardingStep(_ => TopupArweaveWallet)}
      moveToPrevStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
    />
  | Frequency =>
    <Frequency
      moveToNextStep={_ => setOnboardingStep(_ => TopupArweaveWallet)}
      moveToPrevStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
    />
  | TopupArweaveWallet =>
    <div> "List arweave public key to load with tokens"->React.string </div>
  | OnboardingComplete => <div> "DONE"->React.string </div>
  };
};
