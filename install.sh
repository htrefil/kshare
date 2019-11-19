#!/bin/bash

set -e

# Copy binaries.
cp -f build/server/server /usr/bin/kshare-server
cp -f build/client/client /usr/bin/kshare-client

# Copy config files, don't overwrite existing ones if they exist.
mkdir -p /etc/kshare
if [ ! -f /etc/kshare/server.toml ]; then
	cp example/server.toml /etc/kshare/server.toml
fi
if [ ! -f /etc/kshare/client.toml ]; then
	cp example/client.toml /etc/kshare/client.toml
fi
