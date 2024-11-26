from flask import Flask, send_from_directory
from flask_socketio import SocketIO
from utils.logging import logger

from dotenv import load_dotenv
import os

load_dotenv('../.env')

socketio = SocketIO(cors_allowed_origins="*")

def create_app():
    logger.info("Creating app...")
    
    app = Flask(__name__)
    logger.info("Started app successfully")

    app.secret_key = os.getenv("FLASK_SECRET_KEY")
    logger.info(f"Secret loaded as: {app.secret_key[:5]}")

    from . import landing, front, mqtt
    logger.info("Imported fron and mqtt successfully")
    app.register_blueprint(landing.bp)

    app.register_blueprint(front.bp)
    logger.info("Registered front successfully")
    
    app.register_blueprint(mqtt.bp)
    logger.info("Regustered mqtt successfully")
    
    socketio.init_app(app)
    logger.info("Started SocketIO successfully")

    return app
