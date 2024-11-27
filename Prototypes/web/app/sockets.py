from app import socketio
from utils.logging import logger
import json
import random
import uuid

from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify, make_response
)

@socketio.on('connect')
def handle_connect():
    logger.info("Client connected")
    socketio.emit('server_message', {'data': 'Welcome!'})

@socketio.on('ready_for_message')
def check_cookie():
    cookie = request.cookies.get("user_data")
    
    if not cookie:
        new_uuid = str(uuid.uuid4())
        r, g, b = random.randint(50, 180), random.randint(50, 180), random.randint(50, 180)

        user_data = {"uuid": new_uuid, "color": f"#{r:02x}{g:02x}{b:02x}"}
        
        socketio.emit('cookie_update', {'message': 'New cookie generated', 'user_data': user_data})
    else:
        user_data = json.loads(cookie)
        logger.info(f"Existing cookie: {user_data}")
        
        socketio.emit('cookie_valid', {'message': 'Cookie exists','user_data': user_data})
    
    try:
        logger.info("sending from landing page socket")
        socketio.emit("colorchange", {"color": "#3F888F"})
    except Exception as e:
        logger.error(f"failed to emit socket {e}")

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("Client disconnected")