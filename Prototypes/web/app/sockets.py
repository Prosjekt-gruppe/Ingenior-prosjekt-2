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
        logger.info(f"No cookie found")
        socketio.emit('no_cookie', {'message': 'no cookie exists'})
        return
    
    user_data = json.loads(cookie)
    
    try:
        logger.info("sending from landing page socket")
        socketio.emit("colorchange", {"color": user_data['color']})
    except Exception as e:
        logger.error(f"failed to emit socket {e}")

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("Client disconnected")