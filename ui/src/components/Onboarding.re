type onboardingSteps =
  | SkaleEndpoint
  | ArweaveEndpoint
  | GenerateArweaveWallet
  | OnboardingComplete;

module GeneratingArweaveWallet = {
  [@react.component]
  let make = (~moveToNextStep) => {
    <div className="funnel-step-container">
      "step 2"->React.string
      <button onClick=moveToNextStep> "Next"->React.string </button>
    </div>;
  };
};

module CreateArweaveEndpoint = {
  [@react.component]
  let make = (~moveToNextStep) => {
    <div className="funnel-step-container">
      "step 3"->React.string
      <button onClick=moveToNextStep> "Next"->React.string </button>
    </div>;
  };
};
[@react.component]
let make = () => {
  let (onboardingStep, setOnboardingStep) =
    React.useState(_ => SkaleEndpoint);

  switch (onboardingStep) {
  | SkaleEndpoint =>
    <SkaleEndpoint
      moveToNextStep={_ => setOnboardingStep(_ => GenerateArweaveWallet)}
    />
  | GenerateArweaveWallet =>
    <GeneratingArweaveWallet
      moveToNextStep={_ => setOnboardingStep(_ => ArweaveEndpoint)}
    />
  | ArweaveEndpoint =>
    <GeneratingArweaveWallet
      moveToNextStep={_ => setOnboardingStep(_ => OnboardingComplete)}
    />
  | OnboardingComplete => <div> "DONE"->React.string </div>
  };
};
