// module UseInterval = {
// let useInterval(callback, delay) {
//   let savedCallback = React.useRef();
//   // Remember the latest callback.
//   React.useEffect1(() => {
//     savedCallback.current = callback;
//   }, [callback]);
//   // Set up the interval.
//   React.useEffect(() => {
//     let tick = () => {
//       savedCallback.current();
//     }
//     if (delay !== null) {
//       let id = setInterval(tick, delay);
//        () => clearInterval(id);
//     }
//   }, [delay]);
// }
// }
// [@react.component]
// let make = () => {
//   let [inlineLoaderState, setInlineLoaderState] = React.useState(_ => 0);
//   UseInterval.useInterval(async () => {
//     if (inlineLoaderState === 0) {
//       setInlineLoaderState(_ => 1);
//     } else if (inlineLoaderState === 1) {
//       setInlineLoaderState(_ => 2);
//     } else if (inlineLoaderState === 2) {
//       setInlineLoaderState(_ => 3);
//     }
//   }, 300);
//     <>
//       <span style={{ opacity: inlineLoaderState === 0 ? 0.4 : 1 }}>.</span>
//       <span style={{ opacity: inlineLoaderState === 1 ? 0.4 : 1 }}>.</span>
//       <span style={{ opacity: inlineLoaderState === 2 ? 0.4 : 1 }}>.</span>
//     </>
// }
