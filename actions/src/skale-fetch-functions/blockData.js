require("isomorphic-fetch");
const fs = require("fs");
const resolve = require("path").resolve;
const path = require("path");

const absolutePath = path.join(__dirname, "../temp-data/blockData/");

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

      fs.writeFile(absolutePath + filename, jsonContent, "utf8", function (
        err
      ) {
        if (err) {
          console.log("Error here", err);
          onError(err);
        } else {
          callback(absolutePath + filename);
        }
      });
    })
    .catch((err) => {
      onError(err);
    });
};

module.exports = {
  default: fetchData,
};
