#!/bin/bash

SERVER_LOCATION="app/web/"

echo "Building webapp to $SERVER_LOCATION"

if ! [ -d "$SERVER_LOCATION" ]; then
    echo "Creating folder $SERVER_LOCATION"
    mkdir app/web/
fi