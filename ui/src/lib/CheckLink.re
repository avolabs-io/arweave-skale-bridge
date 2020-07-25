/**
 * NOTE: you can use any validation strategy you want for your links here.
 *     - For example, some endpoints may require authentication? How would this be checked? etc.
 *     - You could attempt to run an actual rpc call to check it responds correctly too.
 */

// WARNING: This function is written in plain javascript - (oh the horror)
let checkIsValidLink: (string, bool) => Js.Promise.t((bool, option(string))) = [%raw
  {|
  (inputUrl, checkLink) => {
    const linkCheck = require('link-check');

    let regexQuery = /https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)/gi;

    return new Promise((resolve) => {
      if(inputUrl !== "")
      {
          if (!inputUrl.match(regexQuery)) {
            resolve([false, "Invalid url format (it must include 'http' or 'https')"]);
            return;
          }
      }

      if (checkLink) {
        // NOTE: The `timeout` doesn't seem to work for some reasen.
        linkCheck(inputUrl, {timeout: 20000}, function (err, result) {
            if (err) {
                console.error(err);
                resolve([false, err]);
                return;
            }
            if (result.status === "alive") {
              resolve([true, undefined]);
            } else {
              resolve([false, "The link is dead - it returned an error status of: " + result.statusCode]);
            }
        })
      } else {
        resolve([true, undefined]);
      }
    });
  }
|}
];
