[@bs.get] external getStackTrace: Js.Promise.error => string = "stack";

let errorToMessageAndStacktrace: Js.Promise.error => (string, string) =
  error => (Js.String.make(error), getStackTrace(error));
