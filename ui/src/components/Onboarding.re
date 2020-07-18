type onboardingSteps =
  | Overview
  | SkaleEndpoint
  | ArweaveEndpoint
  | GenerateArweaveWallet
  | TopupArweaveWallet
  | OnboardingComplete;

module Overview = {
  [@react.component]
  let make = (~moveToNextStep) => {
    <div className="funnel-step-container">
      "Steps to take"->React.string
      <p> "1. Specify Skale Endpoint"->React.string </p>
      <p> "1. Specify Arweave endpoint"->React.string </p>
      <button onClick=moveToNextStep> "Get Started"->React.string </button>
    </div>;
  };
};

module GeneratingArweaveWallet = {
  [@react.component]
  let make = (~moveToNextStep, ~moveToPrevStep) => {
    <div className="funnel-step-container">
      "step 2"->React.string
      <button onClick=moveToNextStep> "Next"->React.string </button>
      <button onClick=moveToPrevStep> "Prev"->React.string </button>
    </div>;
  };
};

module CreateArweaveEndpoint = {
  [@react.component]
  let make = (~moveToNextStep, ~moveToPrevStep) => {
    <div className="funnel-step-container">
      "step 3"->React.string
      <button onClick=moveToNextStep> "Next"->React.string </button>
      <button onClick=moveToPrevStep> "Prev"->React.string </button>
    </div>;
  };
};
[@react.component]
let make = () => {
  let (onboardingStep, setOnboardingStep) = React.useState(_ => Overview);

  switch (onboardingStep) {
  | Overview =>
    <Overview moveToNextStep={_ => setOnboardingStep(_ => SkaleEndpoint)} />
  | SkaleEndpoint =>
    <SkaleEndpoint
      moveToNextStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
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
  | TopupArweaveWallet =>
    <div> "List arweave public key to load with tokens"->React.string </div>
  | OnboardingComplete => <div> "DONE"->React.string </div>
  };
};
