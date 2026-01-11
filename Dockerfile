# Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
# All rights reserved.

ARG VARIANT="debug"
ARG LINK="static"

FROM registry.zentrack.cl/stateworks/compiler/base:$VARIANT-$LINK AS builder

ARG BUILD_THREADS=1
ARG VARIANT="debug"
ARG LINK="static"

COPY . .

RUN sh scripts/build.sh $VARIANT $LINK $BUILD_THREADS tests_off

RUN strip --strip-all /srv/build/server

FROM alpine:latest

WORKDIR /srv

COPY --from=builder \
    /srv/build/server \
    /srv/certs/ca.crt \
    /srv/certs/session_listener.crt \
    /srv/certs/session_listener.key \
    /srv/certs/session.key \
    /srv/certs/session.crt \
    /srv/certs/session_dh4096.pem \
    /srv/certs/client_listener.crt \
    /srv/certs/client_listener.key \
    /srv/certs/client.key \
    /srv/certs/client.crt \
    /srv/certs/client_dh4096.pem \
    /srv/

RUN chmod 0444 /srv/ca.crt /srv/client_dh4096.pem /srv/client.crt /srv/client_listener.crt /srv/session_dh4096.pem /srv/session.crt /srv/session_listener.crt \
    && chmod 0400 /srv/client.key /srv/client_listener.key /srv/session.key /srv/session_listener.key \
    && chmod +x /srv/server

EXPOSE 11000
EXPOSE 12000

RUN apk add --no-cache netcat-openbsd

HEALTHCHECK --interval=5s --timeout=5s --retries=3 \
  CMD nc -z localhost 11000 || exit 1

ENTRYPOINT ["/srv/server"]
