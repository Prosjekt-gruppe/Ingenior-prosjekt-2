from app import socketio
from utils.logging import logger

@socketio.on('connect')
def handle_connect():
    logger.info("Client connected")
    socketio.emit('server_message', {'data': 'Welcome!'})

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("Client disconnected")