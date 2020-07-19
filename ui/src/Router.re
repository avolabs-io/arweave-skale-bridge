let useRouter = () => ReasonReactRouter.useUrl()->Route.fromUrl;

let push = (route: Route.t) => route->Route.toString->ReasonReactRouter.push;

module Link = {
  [@react.component]
  let make = (~route: Route.t, ~children) => {
    let location = route->Route.toString;

    <a
      href=location
      onClick={event =>
        // For reference: https://github.com/MinimaHQ/safe-routing-blog-app-example/blob/314634f18c23851236ec1659be421a53cf2abcdd/01-single-type/src/Router.re#L13

          if (!event->ReactEvent.Mouse.defaultPrevented
              && event->ReactEvent.Mouse.button == 0
              && !event->ReactEvent.Mouse.altKey
              && !event->ReactEvent.Mouse.ctrlKey
              && !event->ReactEvent.Mouse.metaKey
              && !event->ReactEvent.Mouse.shiftKey) {
            event->ReactEvent.Mouse.preventDefault;
            location->ReasonReactRouter.push;
          }
        }>
      children
    </a>;
  };
};
