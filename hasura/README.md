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

Restart: (this keeps the database data intact)

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
hasura init --directory my-project --endpoint <hasura endpoint>
```

#### the following needs to be run inside the project folder

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

## Extra

Config hasura to autocomplete when typing in terminal [docs](https://hasura.io/docs/1.0/graphql/manual/hasura-cli/hasura_completion.html#hasura-completion)

```
hasura completion zsh --file=\$HOME/.zprezto/modules/completion/external/src/\_hasura
```
