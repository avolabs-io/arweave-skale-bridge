const fs = require("fs");
const request = require("request");

const url = "http://placekitten.com/g/200/300";
const path = require("path");

const absolutePath = path.join(__dirname, "../temp-data/storageData/");

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
      .pipe(fs.createWriteStream(absoletePath + filename))
      .on("close", () => callback(absoletePath + filename))
      .on("error", (err) => {
        console.log("the error,", err);
        onError(err);
      });
  });
};

module.exports = {
  default: fetchData,
};
