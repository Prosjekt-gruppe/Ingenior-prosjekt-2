from flask import Flask, send_from_directory
from flask_socketio import SocketIO
from utils.logging import logger

from dotenv import load_dotenv
import os

envpath = os.path.abspath('/srv/.env')
socketio = SocketIO(cors_allowed_origins="*")


def create_app():
    logger.info("Creating app...")
    
    app = Flask(__name__)
    logger.info("Started app successfully")

    if os.path.exists(envpath):
        logger.info("Found .env-file")
        load_dotenv('../.env')
        app.secret_key = os.getenv("FLASK_SECRET_KEY")
        logger.info(f"Loaded secret key: {app.secretkey[:5]}")
    else:
        logger.info(f"could not find env-file got {envpath}")


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
