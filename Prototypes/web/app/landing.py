from utils.logging import logger
import uuid
from tinydb import TinyDB, Query
import functools
import random
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
    
    if not cookie:
        new_uuid = str(uuid.uuid4())
        r, g, b = random.randint(50, 180), random.randint(50, 180), random.randint(50, 180)

        user_data = {"uuid": new_uuid, "color": f"#{r:02x}{g:02x}{b:02x}"}

        response = make_response(redirect(url_for('front.front')))
        response.set_cookie('user_data', json.dumps(user_data), max_age=3600)
        return response
    
    user_data = json.loads(cookie)
    
    socketio.emit("colorchange", {"color": user_data["color"]})    
    
    logger.info(f"Sent user_data to socket: {user_data}")

    return redirect(url_for('front.front'))


@socketio.on('connect')
def log_socket_connect():
    logger.info("Landing socket connected")

