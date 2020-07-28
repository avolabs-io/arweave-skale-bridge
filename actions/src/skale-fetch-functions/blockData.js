require("isomorphic-fetch");
const fs = require("fs");

const path = "./src/temp-data/blockData/";

const fetchData = (endpoint, chainId, filename, callback, onError) => {
  fetch(endpoint, {
    method: "post",
    body: JSON.stringify({
      jsonrpc: "2.0",
      method: "eth_blockNumber",
      id: chainId,
    }),
  })
    .then(function (response) {
      return response.json();
    })
    .then(function (data) {
      // Process the data recieved from the RPC endpoint (here we just 'stringify' it)
      var jsonContent = JSON.stringify(data);

      fs.writeFile(path + filename, jsonContent, "utf8", function (err) {
        if (err) {
          console.log("Error here", err);
          onError(err);
        } else {
          callback(path + filename);
        }
      });
    })
    .catch((err) => {
      console.log("Error Catch!!", err);
      onError(err);
    });
};

module.exports = {
  default: fetchData,
};
