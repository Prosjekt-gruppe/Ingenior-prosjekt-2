import os
from flask import Flask, send_from_directory
from flask_socketio import SocketIO
from utils.logging import logger

socketio = SocketIO(cors_allowed_origins="*")

def create_app():
    logger.info("Creating app...")
    
    app = Flask(__name__)
    logger.info("Started app successfully")

    from . import front, mqtt
    logger.info("Imported fron and mqtt successfully")
    

    app.register_blueprint(front.bp)
    logger.info("Registered front successfully")
    
    app.register_blueprint(mqtt.bp)
    logger.info("Regustered mqtt successfully")
    
    socketio.init_app(app)
    logger.info("Started SocketIO successfully")

    return app
