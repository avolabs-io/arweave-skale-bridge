// These bindings are inspired by: https://github.com/JakubMarkiewicz/reason-transition-group

type node = Dom.element;
type addEndListenerFunction = (node, unit => unit) => unit;

module CSSTransition = {
  [@bs.module "react-transition-group/cjs/index"] [@react.component]
  external make:
    (
      ~key: string=?,
      ~classNames: string=?,
      ~addEndListener: addEndListenerFunction=?,
      ~children: React.element
    ) =>
    React.element =
    "CSSTransition";
};

module SwitchTransition = {
  [@bs.module "react-transition-group/cjs/index"] [@react.component]
  external make: (~children: React.element) => React.element =
    "SwitchTransition";
};
