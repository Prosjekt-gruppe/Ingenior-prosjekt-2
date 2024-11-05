#!/bin/bash

venv="venv/"
requirements="requirements.txt"




if [ ! -d "$venv" ]; then
    echo "no venv found trying to setup venv"
    if [ ! -f "$requirements" ]; then
        echo "no requirements file exiting"
        exit 1
    else
        echo "found requirements.txt setting up venv"
        python -m venv .venv
        echo "installing requirements.txt"
        source $venv/bin/activate
        pip install -r requirements.txt
    fi
fi

echo "sourcing env"
source $venv/bin/activate


echo "running flask server"
#gunicorn -w 4 -b 0.0.0.0:8000 --timeout 60 'local:app'
export FLASK_APP=dev
export FLASK_ENV=development
flask run