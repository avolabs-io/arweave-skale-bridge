## Development:

First copy the `.example.env` file to `.env`. Change the passwords ports as desired.

Start Hasura:

```
make start-dev
```

Print Hasura logs:

```
make hasura-logs
```

Restart: (this keepsthedatabase data intact)

```
make restart-dev
```

Restart and clear all data:

```
make hard-restart-dev
```

### Migrations:

`HASURA_GRAPHQL_ENABLE_CONSOLE=false` So that it isn't possible to run migrations via the console.

Install cli: `npm install --global hasura-cli`

Initialise new hasura project (Not needed for general use, this project is already initialised - ie don't run):

```
hasura init --directory my-project --endpoint https://api.wildcards.world
```

#### the following needs to be run inside the project folder (`wildcards` or `wildcards-goerli` for testnet)

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

To connect to the deployed instances you will need to use the below environment variable:

```
HASURA_GRAPHQL_ENDPOINT=https://goerli.api.wildcards.world #or https://api.wildcards.world
```

#### _For production specific instructions see the `PRODUCTION.md` file._

### basic structure:

```
wildcard {
id
name
description
organizationId
ipfsMetadataHash
nftMetadata
imageIpfsHash
launchDate // as unix timestamp?
}
```

```
organisation {
id
name
description
...
}
```

### Other files:

Raw sql scripts should be placed inside the `raw-sql` folder; however they are only used for reference, only the migrations will be used in production.

## Steps for manually adding an animal:

Note, this can be done locally and once the migration is created and working you can apply it to the production db.

1. Run IPFS and upload the animal image and their description to ipfs. Command to start the IPFS node + webui. You can then get the ipfs ui at http://localhost:5001/webui (it takes quite a bit of time to load sometimes...).

```bash
docker run -d --name ipfs_host -v ~/.ipfs-data/export:/export -v ~/.ipfs-data/data:/data/ipfs -p 4001:4001 -p 8099:8080 -p 5001:5001 ipfs/go-ipfs:latest
```

2. Add the details of the animal to the file `./raw-sql/wildcardData.sql`.

3. Run a migration of the latest data via hasura webui that was started via the hasura console.

4. Copy the migration to the `wildcards-goerli` folder too.

5. Save the resulting migrations to git.

6. Run the migrations on mainnet.

## Extra

Config hasura to autocomplete when typing in terminal [docs](https://hasura.io/docs/1.0/graphql/manual/hasura-cli/hasura_completion.html#hasura-completion)

```
hasura completion zsh --file=\$HOME/.zprezto/modules/completion/external/src/\_hasura
```
