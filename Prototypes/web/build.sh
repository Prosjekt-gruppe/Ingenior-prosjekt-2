#!/bin/bash

sudo cp /home/gruppe1/repo/web/index.html /var/www/html/index.html

sudo systemctl restart nginx

echo "completed build"
