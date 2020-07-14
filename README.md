# Skale Arweave bridge

A repo that Skale node maintainers can use to push and backup specific node data on Arweave

### Phase 1:

Load data from the skale chain via the [RPC snapshot endpoint](https://github.com/skalenetwork/skaled/blob/a7d54ed7658609ad96f30eaf33af6e31442e7905/docs/snapshots.md) and push it to Arweave at a predeturmined interval with simple UI.

## Get started

### Backend

Start the backend via docker-compose:
`make start-dev`

### UI

start ui

`cd ui && yarn dev`

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
