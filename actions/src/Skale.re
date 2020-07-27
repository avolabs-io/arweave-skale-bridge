open Globals;
open BridgeSyncTypes;

let processDataFetching =
    (~typeOfDataFetch, ~endpoint, ~onError, {syncItemId}) => {
  // MOCKED FUNCTION.
  Js.log3("Fetching Skale Data", typeOfDataFetch, endpoint);
  Js.Promise.make((~resolve, ~reject as _) => {
    ignore(
      Js.Global.setTimeout(() => resolve(. {syncItemId: syncItemId}), 1000),
    )
  })
  ->Prometo.fromPromise
  ->Prometo.handle(~f=result =>
      switch (result) {
      | Ok(dataFetchResult) => Ok(dataFetchResult)
      | Error(error) =>
        onError(error);
        Error(error);
      }
    );
};
