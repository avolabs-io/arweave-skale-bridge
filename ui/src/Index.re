[%raw "require('./styles/css/global.css')"];

ReactDOMRe.renderToElementWithId(
  <RootProvider>
    <ApolloClient.React.ApolloProvider client=Client.instance>
      <App />
    </ApolloClient.React.ApolloProvider>
  </RootProvider>,
  "root",
);
