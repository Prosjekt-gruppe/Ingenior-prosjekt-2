from app import socketio
from utils.logging import logger
import json
import random
import uuid

from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify, make_response
)

def get_user_data():
    cookie = request.cookies.get("user_data")
    if not cookie:
        new_uuid = str(uuid.uuid4())
        r, g, b = random.randint(50, 180), random.randint(50, 180), random.randint(50, 180)
        user_data = {"uuid": new_uuid, "color": f"#{r:02x}{g:02x}{b:02x}"}
        return user_data, False
    else:
        user_data = json.loads(cookie)
        logger.info(f"Existing cookie: {user_data}")
        return user_data, True


@socketio.on('connect')
def handle_connect():
    logger.info("Client connected")
    socketio.emit('server_message', {'data': 'Welcome!'})

@socketio.on('ready_for_message')
def check_cookie():
    user_data, cookie_exists = get_user_data()
    
    if cookie_exists:
        logger.info(f"Existing cookie: {user_data}")
        socketio.emit('cookie_valid', {'message': 'Cookie exists','user_data': user_data})
    else:
        logger.info(f"generating new cookie: {user_data}")
        socketio.emit('cookie_update', {'message': 'New cookie generated', 'user_data': user_data})
    
    try:
        logger.info("sending from landing page socket")
        socketio.emit("colorchange", {"color": user_data['color']})
    except Exception as e:
        logger.error(f"failed to emit socket {e}")

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("Client disconnected")