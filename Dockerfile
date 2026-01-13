ARG VARIANT="debug"
ARG LINK="static"

FROM ghcr.io/strateworks/compiler:1.90.0-$VARIANT-$LINK AS builder

ARG BUILD_THREADS=4
ARG VARIANT="debug"
ARG LINK="static"

COPY . .

RUN bash scripts/build.sh $VARIANT $LINK $BUILD_THREADS tests_off && \
    strip --strip-all /srv/build/server

FROM alpine:latest

COPY --from=builder \
    /srv/build/server \
    /srv/

COPY --from=builder \
    /srv/build/certificates/ca.crt \
    /srv/build/certificates/client.key \
    /srv/build/certificates/client.crt \
    /srv/build/certificates/client_listener.key \
    /srv/build/certificates/client_listener.crt \
    /srv/build/certificates/session.key \
    /srv/build/certificates/session.crt \
    /srv/build/certificates/session_listener.key \
    /srv/build/certificates/session_listener.crt \
    /srv/build/certificates/dhparams.pem \
    /srv/certificates/

WORKDIR /srv

EXPOSE 11000
EXPOSE 12000

RUN apk add --no-cache netcat-openbsd

HEALTHCHECK --interval=5s --timeout=5s --retries=3 \
  CMD nc -z localhost 11000 || exit 1

ENTRYPOINT ["/srv/server"]

