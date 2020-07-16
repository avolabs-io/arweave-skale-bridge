[%raw "require('./styles/css/global.css')"];

module ApolloProvider = {
  [@react.component]
  let make = (~children) => {
    // <ApolloClient.React.ApolloProvider client=Client.instance>
    children;
    // </ApolloClient.React.ApolloProvider>;
  };
};

ReactDOMRe.renderToElementWithId(
  <RootProvider> <ApolloProvider> <App /> </ApolloProvider> </RootProvider>,
  "root",
);
