import os
from flask import Flask, send_from_directory


def create_app():
    app = Flask(__name__)

    from . import front, mqtt
    app.register_blueprint(front.bp)
    app.register_blueprint(mqtt.bp)
    
#    @app.route('/')
#    def index():
#        return send_from_directory('static', 'index.html')

    return app
