module HiddenAutoFocusButton = {
  [@react.component]
  let make = (~action) => {
    <button
      autoFocus=true
      // className=Css.(style([visibility(`hidden)]))
      onClick={_ => action()}
    />;
  };
};

let%private bounce =
  Css.(
    keyframes([
      (0, [transform(scale(0.1, 0.1)), opacity(0.9)]),
      (60, [transform(scale(1.01, 1.01)), opacity(1.0)]),
      (80, [transform(scale(0.99, 0.99)), opacity(0.9)]),
      (100, [transform(scale(1.0, 1.0)), opacity(1.0)]),
    ])
  );

let selectedItemAnimation =
  Css.(style([animationName(bounce), animationDuration(500)]));
