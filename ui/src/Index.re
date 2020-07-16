[%raw "require('./styles/css/global.css')"];

module ApolloProvider = {
  [@react.component]
  let make = (~children) => {
    let headers = RootProvider.useHeaders();
    <ApolloClient.React.ApolloProvider
      client={Client.useGlobalApolloInstance(headers)}>
      children
    </ApolloClient.React.ApolloProvider>;
  };
};

ReactDOMRe.renderToElementWithId(
  <RootProvider> <ApolloProvider> <App /> </ApolloProvider> </RootProvider>,
  "root",
);
