let useReadableRouteName = () => {
  let route = Router.useRouter();
  switch (route) {
  | None
  | Some(Main)
  | Some(Bridge(_))
  | Some(CreateBridge)
  | Some(About)
  | Some(GqlExamplesPage) => None
  | Some(Config) => "Config Page"->Some
  | Some(Profile) => "Profile Profile"->Some
  | Some(Dashboard) => "Dashboard"->Some
  };
};

[@react.component]
let make = (~moveToNextStep) => {
  let pageName = useReadableRouteName();
  <div className="funnel-step-container overview">
    {switch (pageName) {
     | Some(pageName) =>
       <>
         <p>
           <em>
             <strong> "Please setup your first bridge."->React.string </strong>
           </em>
         </p>
         <p>
           <em>
             <strong>
               {j|After that you can view your $pageName|j}->React.string
             </strong>
           </em>
         </p>
         <br />
       </>
     | None => React.null
     }}
    <h2> "Overview"->React.string </h2>
    <h4> {js| 👉🏽 |js}->React.string "Step 1"->React.string </h4>
    <p> "- Specify Skale Endpoint"->React.string </p>
    <h4> {js| 👉🏽 |js}->React.string "Step 2"->React.string </h4>
    <p> "- Select Data Type"->React.string </p>
    <h4> {js| 👉🏽 |js}->React.string "Step 3"->React.string </h4>
    <p> "- Set frequency"->React.string </p>
    <h4> {js| 👉🏽 |js}->React.string "Step 4"->React.string </h4>
    <p> "- Specify Arweave endpoint"->React.string </p>
    <h4> {js| 👉🏽 |js}->React.string "Step 5"->React.string </h4>
    <p> "- Fund Arweave wallet"->React.string </p>
    <button onClick=moveToNextStep className="full-width-button">
      "Get Started"->React.string
    </button>
  </div>;
};
