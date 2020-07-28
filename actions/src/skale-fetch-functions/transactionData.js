const fs = require("fs");
const request = require("request");
const resolve = require("path").resolve;

const url = "https://loremflickr.com/320/240";
const path = require("path");

const absolutePath = path.join(__dirname, "../temp-data/transactionData/");

const fetchData = (endpoint, chainId, filename, callback, onError) => {
  console.log(
    "We should be fetching 'event data' from:",
    endpoint,
    "wich chain id:",
    chainId
  );

  request.head(url, (err, res, body) => {
    if (!!err) {
      onError("there was an error!");
    }

    request(url)
      .pipe(fs.createWriteStream(absolutePath + filename))
      .on("close", () => callback(absolutePath + filename))
      .on("error", (err) => {
        console.log("the error,", err);
        onError(err);
      });
  });
};

module.exports = {
  default: fetchData,
};
