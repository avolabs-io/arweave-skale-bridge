open Globals;

let processDataFetching = (~typeOfDataFetch, ~endpoint, ~onError) => {
  // MOCKED FUNCTION.
  Js.log3("Fetching Skale Data", typeOfDataFetch, endpoint);
  Js.Promise.make((~resolve, ~reject as _) => {
    ignore(
      Js.Global.setTimeout(
        () =>
          resolve(.
            "Finished Featching Skale Data - this is some test data that must be pushed to arweave...",
          ),
        1000,
      ),
    )
  })
  ->Prometo.fromPromise
  ->Prometo.handle(~f=result =>
      switch (result) {
      | Ok(_) => Ok(3)
      | Error(error) =>
        onError(error);
        Error(error);
      }
    );
};
