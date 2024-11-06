from utils.logging import logger
from flask import Blueprint, request, jsonify
bp = Blueprint('mqtt', __name__, url_prefix='/mqtt')


mqttdata = []

@bp.route('/', methods=['POST'])
def getmqtt():
    data = request.get_json()
    topic = data.get('topic')
    message = data.get('message')
    device = data.get('device_id')
    
    logger.info(f"Received POST on /mqtt, topic: {topic}, message: {message}")
    
    mqttdata.append({'topic': topic, 'message': message, 'device': device})
    return jsonify({"status": "success"}), 200

def getdata():
    logger.info(f"returning mqttdata: {mqttdata}")
    return mqttdata