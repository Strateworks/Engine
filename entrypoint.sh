#!/bin/sh
set -e

cp "$CA_CRT"                  /srv/ca.crt
cp "$CLIENT_DH"               /srv/client_dh4096.pem
cp "$CLIENT_KEY"              /srv/client.key
cp "$CLIENT_CRT"              /srv/client.crt
cp "$CLIENT_LISTENER_KEY"     /srv/client_listener.key
cp "$CLIENT_LISTENER_CRT"     /srv/client_listener.crt
cp "$SESSION_DH"              /srv/session_dh4096.pem
cp "$SESSION_KEY"             /srv/session.key
cp "$SESSION_CRT"             /srv/session.crt
cp "$SESSION_LISTENER_KEY"    /srv/session_listener.key
cp "$SESSION_LISTENER_CRT"    /srv/session_listener.crt

chmod 0444 \
  /srv/ca.crt \
  /srv/client_dh4096.pem \
  /srv/client.crt \
  /srv/client_listener.crt \
  /srv/session_dh4096.pem \
  /srv/session.crt \
  /srv/session_listener.crt

chmod 0400 \
  /srv/client.key \
  /srv/client_listener.key \
  /srv/session.key \
  /srv/session_listener.key

exec /srv/server "$@"