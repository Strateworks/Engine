# Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
# All rights reserved.

#!/usr/bin/env bash
set -euo pipefail

OUTDIR="$(cd "$(dirname "$0")" && pwd)/../build"
mkdir -p "$OUTDIR"

HOST=${1:-"localhost"}
CA_PASSWORD=${2:-"4df8489a"}
SESSION_LISTENER_PASSWORD=${3:-"610191e8"}
SESSION_PASSWORD=${4:-"5ec35a12"}
CLIENT_LISTENER_PASSWORD=${5:-"36e422f3"}
CLIENT_PASSWORD=${6:-"d96ab300"}

# This will generate:
# - ca.key PRIVATE
# - ca.crt PUBLIC
# - session_dh4096.pem PUBLIC
# - session_listener.key PRIVATE
# - session_listener.csr TRANSITORY
# - session_listener.crt PUBLIC
# - session.key PRIVATE
# - session.csr TRANSITORY
# - session.crt PUBLIC
# - client_dh4096.pem PUBLIC
# - client_listener.key PRIVATE
# - client_listener.csr TRANSITORY
# - client_listener.crt PUBLIC
# - client.key PRIVATE
# - client.csr TRANSITORY
# - client.crt PUBLIC

# Generate a CA certificate:
openssl req -new -newkey rsa:4096 -keyout "$OUTDIR/ca.key" -x509 -out "$OUTDIR/ca.crt" -subj "/CN=$HOST" -days 365 -passout pass:"$CA_PASSWORD"

## Session

# Generate a Session Listener CSR:
openssl req -new -newkey rsa:4096 -keyout "$OUTDIR/session_listener.key" -out session_listener.csr -subj "/CN=$HOST" -addext "subjectAltName=DNS:$HOST" -passout pass:"$SESSION_LISTENER_PASSWORD"

# Sign the Session Listener CSR using our CA:
openssl x509 -req -in session_listener.csr -CA "$OUTDIR/ca.crt" -CAkey "$OUTDIR/ca.key" -copy_extensions copy -days 365 -out "$OUTDIR/session_listener.crt" -passin pass:"$CA_PASSWORD"

# Generate a Session CSR:
openssl req -new -newkey rsa:4096 -keyout "$OUTDIR/session.key" -out session.csr -subj "/CN=session" -passout pass:"$SESSION_PASSWORD"

# Sign the Session CSR using our CA:
openssl x509 -req -in session.csr -CA "$OUTDIR/ca.crt" -CAkey "$OUTDIR/ca.key" -days 365 -out "$OUTDIR/session.crt" -passin pass:"$CA_PASSWORD"

# Generate a Session DH parameters file:
openssl dhparam -out "$OUTDIR/session_dh4096.pem" 4096

## Client

# Generate a Client Listener CSR:
openssl req -new -newkey rsa:4096 -keyout "$OUTDIR/client_listener.key" -out client_listener.csr -subj "/CN=$HOST" -addext "subjectAltName=DNS:$HOST" -passout pass:"$CLIENT_LISTENER_PASSWORD"

# Sign the Client Listener CSR using our CA:
openssl x509 -req -in client_listener.csr -CA "$OUTDIR/ca.crt" -CAkey "$OUTDIR/ca.key" -copy_extensions copy -days 365 -out "$OUTDIR/client_listener.crt" -passin pass:"$CA_PASSWORD"

# Generate a Client CSR:
openssl req -new -newkey rsa:4096 -keyout "$OUTDIR/client.key" -out client.csr -subj "/CN=client" -passout pass:"$CLIENT_PASSWORD"

# Sign the Client CSR using our CA:
openssl x509 -req -in client.csr -CA "$OUTDIR/ca.crt" -CAkey "$OUTDIR/ca.key" -days 365 -out "$OUTDIR/client.crt" -passin pass:"$CA_PASSWORD"

# Generate a Client DH parameters file:
openssl dhparam -out "$OUTDIR/client_dh4096.pem" 4096
