const fs = require("fs");
const request = require("request");

const url = "https://thispersondoesnotexist.com/image";
const path = "./src/temp-data/eventData/";

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
      .pipe(fs.createWriteStream(path + filename))
      .on("close", () => callback(path + filename))
      .on("error", (err) => {
        console.log("the error,", err);
        onError(err);
      });
  });
};

module.exports = {
  default: fetchData,
};
