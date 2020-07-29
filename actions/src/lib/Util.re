[@bs.get]
external getStackTrace: Js.Promise.error => option(string) = "stack";

let errorToMessageAndStacktrace: Js.Promise.error => (string, option(string)) =
  error => (Js.String.make(error), getStackTrace(error));
