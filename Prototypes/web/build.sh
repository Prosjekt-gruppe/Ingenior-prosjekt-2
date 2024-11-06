#!/bin/bash

# move new files if modified
sudo rsync -av /srv/app/static/index.html /var/www/html/index.html
sudo rsync -av /srv/nginx/default /etc/nginx/sites-enabled/default
sudo rsync -av /srv/gunicorn/gunicorn.service /etc/systemd/system/gunicorn.service

prj_path="/srv"
venv="${prj_path}/venv"
requirements="${prj_path}/requirements.txt"

if [ ! -d "$venv" ]; then
    echo "no venv found trying to setup venv"
    if [ ! -f "$requirements" ]; then
        echo "no requirements file exiting"
        exit 1
    else
        echo "found requirements.txt setting up venv"
        python3 -m venv $venv
        echo "installing requirements.txt"
        source $venv/bin/activate
        pip3 install -r $requirements
    fi
fi

sudo systemctl daemon-reload
sudo systemctl restart gunicorn
sudo systemctl restart nginx
echo "completed build"

