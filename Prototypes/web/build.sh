#!/bin/bash

sudo cp /home/gruppe1/web/index.html /var/nginx/html/index.html

sudo systemctl restart nginx

echo "completed build"
