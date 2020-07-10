# Skale Arweave bridge

A repo that Skale node maintainers can use to push and backup specific node data on Arweave

### Phase 1:

Load data from the skale chain via the [RPC snapshot endpoint](https://github.com/skalenetwork/skaled/blob/a7d54ed7658609ad96f30eaf33af6e31442e7905/docs/snapshots.md) and push it to Arweave at a predeturmined interval with simple UI.

## Get started

`make start`

start ui

`cd ui && yarn dev`

build ui

`cd ui && yarn build`

big dev helper for compile time errors

`re:watch`

## Under the hood / Behind the scenes

- [Skale Network](https://skale.network/)
- [Arweave](https://www.arweave.org/)
- [ArweaveJS](https://github.com/ArweaveTeam/arweave-js)
- [Vite - Lightning fast es6 build tool](https://github.com/vitejs/vite/)
- [Reason](https://reasonml.github.io/)
- [Reason-React](https://github.com/reasonml/reason-react)
