[@react.component]
let make = () => {
  <div id="about">
    <h1> "About the Arweave Skale Bridge"->React.string </h1>
    <p>
      "This application is a bridge between "->React.string
      <a href="https://skale.network"> "Skale"->React.string </a>
      " and "->React.string
      <a href="https://arweave.org"> "Arweave"->React.string </a>
      ". It is intended to be used by skale node maintainers to backup data from Skale onto Arweave on an automated recurring basis."
      ->React.string
    </p>
    // <p> "."->React.string </p>
    <p>
      "The tool is built using "->React.string
      <a href="https://reasonml.github.io/"> "ReasonML, "->React.string </a>
      <a href="https://reasonml.github.io/reason-react/">
        "Reason React, "->React.string
      </a>
      <a href="https://hasura.io/"> "Hasura, "->React.string </a>
      <a href="https://skale.network/"> "Skale, "->React.string </a>
      "and "->React.string
      <a href="https://arweave.org/"> "Arweave. "->React.string </a>
      "The app can be self hosted for maximum security. You can follow the instructions on "
      ->React.string
      <a href="https://github.com/avolabs-io/arweave-skale-bridge">
        "Github "->React.string
      </a>
      <img src="/assets/icons/github-white.svg" height="30px" />
      " for more information."->React.string
    </p>
  </div>;
};
