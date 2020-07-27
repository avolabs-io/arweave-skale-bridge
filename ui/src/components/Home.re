open Router;
[@react.component]
let make = () => {
  <div id="home">
    <div className="heading-container">
      <h1> "Dashboard"->React.string </h1>
      <button
        onClick={_ => Route.CreateBridge->Router.push}
        className="secondary-button">
        <Link route=CreateBridge>
          "Create Bridge "->React.string
          {js| 🌉 |js}->React.string
        </Link>
      </button>
    </div>
    <div className="dashboard-container"> <Profile /> <Bridges /> </div>
  </div>;
};
