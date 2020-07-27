const fs = require("fs");
const request = require("request");

const url = "https://picsum.photos";
const path = "./blockData/";

const fetchData = (endpoint, filename, callback, onError) => {
  request.head(url, (err, res, body) => {
    request(url)
      .pipe(fs.createWriteStream(path))
      .on("close", callback)
      .on("error", onError);
  });
};

module.exports = {
  default: fetchData,
};
