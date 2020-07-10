FROM node:12.16.3-alpine3.9

WORKDIR /app

RUN apk update
RUN apk upgrade
RUN apk add git python make g++ bash

# Needed to make ppx's run correctly: see https://github.com/reason-association/genType/issues/346#issuecomment-574756981
RUN wget -q -O /etc/apk/keys/sgerrand.rsa.pub https://alpine-pkgs.sgerrand.com/sgerrand.rsa.pub && \
  wget https://github.com/sgerrand/alpine-pkg-glibc/releases/download/2.25-r0/glibc-2.25-r0.apk && \
  apk add glibc-2.25-r0.apk

ADD ./package.json /app/package.json
ADD ./yarn.lock /app/yarn.lock

RUN yarn install

ADD . /app

RUN yarn re:build

# Run an empty command by default - to keep container running
CMD yarn start
