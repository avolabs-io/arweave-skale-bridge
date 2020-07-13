// TODO: this uses onegraph for login, which is convenient. Unfortunately it had some bugs!
import OneGraphAuth from "onegraph-auth";

import React, { Component } from "react";
// import logo from "./logo.svg";
// import "./App.css";

const APP_ID = "80d287f5-a514-46ca-a17a-caa263b8e2cb";

class App extends Component {
  state = {
    isLoggedIn: false,
  };

  constructor(props) {
    super(props);
    this._oneGraphAuth = new OneGraphAuth({
      appId: APP_ID,
    });
  }

  _authWithYoutube = async () => {
    await this._oneGraphAuth.login("github");
    const isLoggedIn = await this._oneGraphAuth.isLoggedIn("github");
    this.setState({ isLoggedIn: isLoggedIn });
  };

  componentDidMount() {
    this._oneGraphAuth
      .isLoggedIn("github")
      .then((isLoggedIn) => this.setState({ isLoggedIn }));
  }

  render() {
    return (
      <div className="App">
        <header className="App-header">
          <h1 className="App-title">YouTube Captions</h1>
        </header>
        <p className="App-intro">
          {this.state.isLoggedIn ? (
            <span>You are logged in with YouTube</span>
          ) : (
            <button style={{ fontSize: 18 }} onClick={this._authWithYoutube}>
              Login with YouTube
            </button>
          )}
        </p>
        <p>
          auth headers::{JSON.stringify(this._oneGraphAuth.authHeaders())}::
        </p>
      </div>
    );
  }
}

export default App;
