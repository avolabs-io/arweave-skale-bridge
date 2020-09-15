**_[Live example](http://34.65.171.221:3000)_**

# Bridge - Persist data from Skale to Arweave [![Tweet](https://img.shields.io/twitter/url/http/shields.io.svg?style=social)](https://twitter.com/intent/tweet?text=Persisting%20my%20Skale%20node%20date%20easily%20to%20Arweave%20using%20Bridge.%20Scalability%20is%20here.&url=https://github.com/avolabs-io/arweave-skale-bridge&hashtags=skale,arweave,persist)

<img src="https://github.com/avolabs-io/arweave-skale-bridge/blob/master/ui/public/assets/preview.png"  alt="Preview image"/>

Need info? Join the Skale [Discord](https://discord.gg/vvutwjb)
| or [Create an issue](https://github.com/avolabs-io/arweave-skale-bridge/issues/new)
| Built with :green_heart: by [Avo Labs](https://avolabs.io)

[![GitHub issues](https://img.shields.io/github/issues-raw/avolabs-io/arweave-skale-bridge?style=flat)](https://github.com/avolabs-io/arweave-skale-bridge/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/avolabs-io/arweave-skale-bridge)](https://github.com/avolabs-io/arweave-skale-bridge/pulls)
[![License](https://img.shields.io/github/license/avolabs-io/arweave-skale-bridge.svg)](license.txt)
![Contributions welcome](https://img.shields.io/badge/contributions-welcome-orange.svg)
[![Awesome](https://cdn.rawgit.com/sindresorhus/awesome/d7305f38d29fed78fa85652e3a63e154dd8e8829/media/badge.svg)](https://github.com/sindresorhus/awesome)

- [Summary](#summary)
- [Backend](#backend)
- [Hasura](#hasura)
  - [Development](#development)
  - [Migrations](#migrations)
  - [Extra](#extra)
- [UI](#ui)
- [Under the hood / Behind the scenes](#under-the-hood---behind-the-scenes)

# Summary

The Bridge project is a tool allowing Skale node operators to easily persist 'snapshots' of Skale sub-chains at a specified frequency. The data is uploaded to Arweave, a decentralized solution for eternal storage.

How the bridge works? It Loads data from the skale chain via the [RPC snapshot endpoint](https://github.com/skalenetwork/skaled/blob/a7d54ed7658609ad96f30eaf33af6e31442e7905/docs/snapshots.md) and pushes it to Arweave at a predetermined interval with simple UI.

# Backend

Start the backend via docker-compose:
`make start-dev`

When hasura is in a bad state :)
`make hard-restart-dev`

# Hasura

### Development

First copy the `.example.env` file to `.env`. Change the passwords ports as desired.

Start Hasura:

```
make start-dev
```

Print Hasura logs:

```
make hasura-logs
```

Restart: (this keeps the database data intact)

```
make restart-dev
```

Restart and clear all data:

```
make hard-restart-dev
```

### Migrations

`HASURA_GRAPHQL_ENABLE_CONSOLE=false` So that it isn't possible to run migrations via the console.

Install cli: `npm install --global hasura-cli`

Initialise new hasura project (Not needed for general use, this project is already initialised - ie don't run):

```
hasura init --directory my-project --endpoint <hasura-endpoint>
```

Run migrations:

(Please add a space at the beginning of this line so that it doesn't save the password into your bash-history. We will find a better way to handle this when we are more established.)

```
hasura migrate apply --admin-secret <your password>
```

Access the hasura console:

```
hasura console --admin-secret <your password>
```

**Note, you can use an env file to avoid the need for typing passwords in the terminal. You specify the env file using `--envfile <path-to-env-file>`.**

### Extra

Config hasura to autocomplete when typing in terminal [docs](https://hasura.io/docs/1.0/graphql/manual/hasura-cli/hasura_completion.html#hasura-completion)

```
hasura completion zsh --file=\$HOME/.zprezto/modules/completion/external/src/\_hasura
```

# UI

start ui

`cd ui && yarn dev`

build scss (needed when creating a new scss file)

`cd ui && yarn scss`

build ui

`cd ui && yarn build`

big dev helper for compile time errors

`cd ui && re:watch`

If and when you ever make changes to the graphql schema you should update the file called `graphql_schema.json` which is autogenerated via
`cd ui && yarn update-schema`.
This file should be commited to git.

# Under the hood / Behind the scenes

- [Skale Network](https://skale.network/)
- [Arweave](https://www.arweave.org/)
- [ArweaveJS](https://github.com/ArweaveTeam/arweave-js)
- [Reason](https://reasonml.github.io/)
- [Reason-React](https://github.com/reasonml/reason-react)
