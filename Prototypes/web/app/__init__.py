from flask import Flask, send_from_directory
from flask_socketio import SocketIO
from flask_limiter import Limiter
from flask_limiter.util import get_remote_address
from utils.logging import logger

from dotenv import load_dotenv
import os

envpath = os.path.abspath('/srv/.env')
socketio = SocketIO(cors_allowed_origins="*")
limiter = Limiter(get_remote_address)

def create_app():
    logger.info("Creating app...")
    
    app = Flask(__name__)
    logger.info("Started app successfully")

    if os.path.exists(envpath):
        logger.info("Found .env-file")
        load_dotenv(envpath)
        app.secret_key = os.getenv("FLASK_SECRET_KEY")
        logger.info(f"Loaded secret key: {app.secret_key[:5]}")
    else:
        logger.info(f"could not find env-file got {envpath}")

    limiter.init_app(app)
    logger.info("limiter set")

    socketio.init_app(app)
    logger.info("Started SocketIO successfully")

    from . import landing, front, mqtt
    logger.info("Imported fron and mqtt successfully")
    app.register_blueprint(landing.bp)

    app.register_blueprint(front.bp)
    logger.info("Registered front successfully")
    
    app.register_blueprint(mqtt.bp)
    logger.info("Regustered mqtt successfully")

    from . import sockets
    logger.info("Socket import success")

    return app
