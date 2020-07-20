# Bridge - Persist data from Skale to Arweave [![Tweet](https://img.shields.io/twitter/url/http/shields.io.svg?style=social)](https://twitter.com/intent/tweet?text=Persisting%20my%20Skale%20node%20date%20easily%20to%20Arweave%20using%20Bridge.%20Scalability%20is%20here.&url=https://github.com/avolabs-io/arweave-skale-bridge&hashtags=skale,arweave,persist)

<img src="https://github.com/avolabs-io/arweave-skale-bridge/blob/master/ui/public/assets/preview.png"  alt="Preview image"/>

Need info? Join the Skale [Discord](https://discord.gg/vvutwjb)
| or [Create an issue](https://github.com/avolabs-io/arweave-skale-bridge/issues/new)
| Built with <3 by [Avo Labs](https://avolabs.io)

[![GitHub issues](https://img.shields.io/github/issues-raw/avolabs-io/arweave-skale-bridge?style=flat)](https://github.com/avolabs-io/arweave-skale-bridge/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/avolabs-io/arweave-skale-bridge)](https://github.com/avolabs-io/arweave-skale-bridge/pulls)
[![License](https://img.shields.io/github/license/avolabs-io/arweave-skale-bridge.svg)](license.txt)
[![Awesome](https://cdn.rawgit.com/sindresorhus/awesome/d7305f38d29fed78fa85652e3a63e154dd8e8829/media/badge.svg)](https://github.com/sindresorhus/awesome)

## Skale Arweave bridge

A repo that Skale node maintainers can use to push and backup specific node data on Arweave

### Phase 1:

Load data from the skale chain via the [RPC snapshot endpoint](https://github.com/skalenetwork/skaled/blob/a7d54ed7658609ad96f30eaf33af6e31442e7905/docs/snapshots.md) and push it to Arweave at a predeturmined interval with simple UI.

## Get started

### Backend

Start the backend via docker-compose:
`make start-dev`

When hasura is in a bad state :)
`make hard-restart-dev`

### UI

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

## Under the hood / Behind the scenes

- [Skale Network](https://skale.network/)
- [Arweave](https://www.arweave.org/)
- [ArweaveJS](https://github.com/ArweaveTeam/arweave-js)
- [Reason](https://reasonml.github.io/)
- [Reason-React](https://github.com/reasonml/reason-react)
