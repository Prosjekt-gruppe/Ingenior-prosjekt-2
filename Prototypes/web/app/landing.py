from utils.logging import logger
import uuid
from tinydb import TinyDB, Query
import functools
import random as r
import json


from flask_limiter.util import get_remote_address
from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify, make_response
)

# local imports
from app import socketio
from . import limiter

# db
db = TinyDB('uuids.json')
UUID = Query()

# blueprints
bp = Blueprint('landing', __name__)

# send user to front
@bp.route('/')
@limiter.limit("5 per minute")
def redirect_front():
    cookie = request.cookies.get("user_data")
    
    if not user_data:
        new_uuid = uuid.uuid4
        r,g,b = r.randint(50, 180), r.randint(50, 180), r.randint(50, 180)

        user_data = {"uuid":new_uuid, "color": f"#{r:02x}{g:02x}{b:02x}"}

        response = make_response(redirect(url_for('front.front')))
        response.set_cookie('uuid', json.dumps(user_data), max_age=3600)
        return response
    
    user_data = json.load(cookie)
    socketio.emit("colorchange", {"color": user_data['color']})

    return redirect(url_for('front.front'))