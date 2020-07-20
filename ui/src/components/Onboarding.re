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
let make =
    (
      ~defaultSkaleEndpointInput=None,
      ~defaultSkaleDataTypeInput=None,
      ~defaultFrequencyInput=None,
      ~defaultArweaveEndpointInput=None,
    ) => {
  let (onboardingStep, setOnboardingStep) =
    React.useState(_ => ArweaveEndpoint); //TODO remember to change this back to Overview
  let (skaleEndpointInput, setSkaleEndpointInput) =
    React.useState(_ => defaultSkaleEndpointInput);
  let (skaleDataTypeInput, setSkaleDataType) =
    React.useState(_ => defaultSkaleDataTypeInput);
  let (frequencyInput, setFrequencyInput) =
    React.useState(_ => defaultFrequencyInput);
  let (arveaweEndpointInput, setArweaveEndpointInput) =
    React.useState(_ => defaultArweaveEndpointInput);
  let (arweaveAddress, setArweaveAddress) = React.useState(_ => None);

  [%log.warn
    "INFO: current onboarding state";
    ("skale endpoint id", skaleEndpointInput);
    ("skale datatype", skaleDataTypeInput);
    ("frequency Input", frequencyInput);
    ("Arveawe ENdpoint", arveaweEndpointInput)
  ];

  switch (onboardingStep) {
  | Overview =>
    <Overview moveToNextStep={_ => setOnboardingStep(_ => SkaleEndpoint)} />
  | SkaleEndpoint =>
    <SkaleEndpoint
      setSkaleEndpointInput
      moveToPrevStep={_ => setOnboardingStep(_ => Overview)}
      moveToNextStep={_ => {setOnboardingStep(_ => SkaleDataTypeToStore)}}
    />
  | SkaleDataTypeToStore =>
    <SkaleDataTypeToStore
      setSkaleDataType
      skaleDataTypeInput
      moveToPrevStep={_ => setOnboardingStep(_ => SkaleEndpoint)}
      moveToNextStep={_ => setOnboardingStep(_ => Frequency)}
    />
  | Frequency =>
    <Frequency
      frequencyInput
      setFrequencyInput
      moveToPrevStep={_ => setOnboardingStep(_ => SkaleDataTypeToStore)}
      moveToNextStep={_ => setOnboardingStep(_ => ArweaveEndpoint)}
    />
  | ArweaveEndpoint =>
    <ArweaveEndpoint
      arveaweEndpointInput
      setArweaveEndpointInput
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
