const fs = require("fs");
const request = require("request");

const url = "https://thispersondoesnotexist.com/image";
// const url = "https://picsum.photos/200/300";
const path = "./src/temp-data/blockData/";

const fetchData = (endpoint, filename, callback, onError) => {
  console.log("We should be fetching 'block data' from:", endpoint);

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
