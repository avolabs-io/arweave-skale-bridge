[%raw "require('../styles/css/create-stream.css')"];
open Globals;

module GeneratingArweaveWallet = {
  [@react.component]
  let make = (~moveToNextStep, ~moveToPrevStep) => {
    <div className="funnel-step-container">
      "step 2"->React.string
      <NavigationButtons moveToNextStep moveToPrevStep nextDisabled=false />
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
  open Route;
  let (onboardingStep, setOnboardingStep) = React.useState(_ => Overview);
  let (skaleEndpointInput, setSkaleEndpointInput) =
    React.useState(_ => defaultSkaleEndpointInput);
  let (skaleDataTypeInput, setSkaleDataType) =
    React.useState(_ => defaultSkaleDataTypeInput);
  let (frequencyInput, setFrequencyInput) =
    React.useState(_ => defaultFrequencyInput);
  let (arweaveEndpointInput, setArweaveEndpointInput) =
    React.useState(_ => defaultArweaveEndpointInput);
  let (arweaveAddress, setArweaveAddress) = React.useState(_ => None);

  [%log.warn
    "INFO: current onboarding state";
    ("skale endpoint id", skaleEndpointInput);
    ("skale datatype", skaleDataTypeInput);
    ("frequency Input", frequencyInput);
    ("Arveawe ENdpoint", arweaveEndpointInput);
    ("Onboarding Step", onboardingStep)
  ];
  open ReactTransition;
  let addEndListener: addEndListenerFunction = [%raw
    {|
        (node, done) => {
          node.addEventListener("transitionend", done, false);
        }
      |}
  ];
  <SwitchTransition>
    <CSSTransition
      key={onboardingStep->Obj.magic->string_of_int}
      addEndListener
      classNames="onboarding-step-transition">
      {switch (onboardingStep) {
       | Overview =>
         <Overview
           moveToNextStep={() => setOnboardingStep(_ => {SkaleEndpoint})}
         />
       | SkaleEndpoint =>
         <SkaleEndpoint
           skaleEndpointInput
           setSkaleEndpointInput
           moveToPrevStep={() => setOnboardingStep(_ => {Overview})}
           moveToNextStep={() => {
             setOnboardingStep(_ => SkaleDataTypeToStore)
           }}
         />
       | SkaleDataTypeToStore =>
         <SkaleDataTypeToStore
           setSkaleDataType
           skaleDataTypeInput
           moveToPrevStep={() => setOnboardingStep(_ => SkaleEndpoint)}
           moveToNextStep={() => setOnboardingStep(_ => Frequency)}
         />
       | Frequency =>
         <Frequency
           frequencyInput
           setFrequencyInput
           moveToPrevStep={() => setOnboardingStep(_ => SkaleDataTypeToStore)}
           moveToNextStep={() => setOnboardingStep(_ => ArweaveEndpoint)}
         />
       | ArweaveEndpoint =>
         <ArweaveEndpoint
           arweaveEndpointInput
           setArweaveEndpointInput
           moveToPrevStep={() => setOnboardingStep(_ => Frequency)}
           moveToNextStep={() => setOnboardingStep(_ => TopupArweaveWallet)}
         />
       | TopupArweaveWallet =>
         <ArweaveWalletGen
           setArweaveAddress
           // arweaveAddress
           moveToPrevStep={() => setOnboardingStep(_ => ArweaveEndpoint)}
           moveToNextStep={() => setOnboardingStep(_ => OnboardingComplete)}
         />
       | OnboardingComplete =>
         <BridgeSubmitted
           skaleEndpointInput
           skaleDataTypeInput
           frequencyInput={frequencyInput->Option.map(Frequency.timeToSecond)}
           arweaveEndpointInput
           goToStep={step => setOnboardingStep(_ => step)}
         />
       }}
    </CSSTransition>
  </SwitchTransition>;
};
