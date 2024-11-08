from utils.logging import logger
from flask import Blueprint, request, jsonify
from app import socketio
bp = Blueprint('mqtt', __name__, url_prefix='/mqtt')


mqttdata = []



@bp.route('/', methods=['POST'])
def getmqtt():
    logger.info("Got post request")

    data = request.get_json()
    topic = data.get('topic')
    device = data.get('device')
    message = data.get('message')
    
    logger.info(f"Received POST on /mqtt, topic: {topic}, message: {message}")
    
    mqttdata.append({'topic': topic, 'message': message, 'device': device})
    
    socketio.emit("mqttsocket", {'topic': topic, 'message': message, 'device': device})

    return jsonify({"status": "success"}), 200

def getdata():
    logger.info(f"returning mqttdata: {mqttdata}")
    return mqttdata

@socketio.on('connect')
def log_socket_connect():
    logger.info("Socket connected")

