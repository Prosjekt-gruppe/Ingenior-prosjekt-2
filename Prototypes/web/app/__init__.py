import os
from flask import Flask, send_from_directory
from flask_socketio import SocketIO

socketio = SocketIO(cors_allowed_origins="*")

def create_app():
    app = Flask(__name__)

    from . import front, mqtt
    app.register_blueprint(front.bp)
    app.register_blueprint(mqtt.bp)
    
    
    socketio.init_app(app)

    return app
