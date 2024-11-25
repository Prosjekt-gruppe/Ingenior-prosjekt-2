from utils.logging import logger
from flask import Blueprint, request, jsonify
from app import socketio
from tinydb import TinyDB, Query
bp = Blueprint('mqtt', __name__, url_prefix='/mqtt')

db = TinyDB('db/nfctags.json')

#mqttdata = []

def find_poi(nfctagID):
    res = db.search(Query().nfctagID == nfctagID)
    logger.info(f"From find_poi: got {res}")
    
    if res:
        return res[0]["poiID"]
    else:
        return None

@bp.route('/', methods=['POST'])
def getmqtt():
    logger.info("Got post request")

    data = request.get_json()
    topic = data.get('topic')
    device = data.get('device')
    message = data.get('message')
    
    logger.info(f"Received POST on /mqtt, topic: {topic}, message: {message}")
    
#   mqttdata.append({'topic': topic, 'message': message, 'device': device})
    
    socketio.emit("mqttsocket", {'topic': topic, 'message': message, 'device': device})

    return jsonify({"status": "success"}), 200

@bp.route('/location', methods=['POST'])
def getmqtt():
    logger.info("Got post request")

    data = request.get_json()
    topic = data.get('topic')
    device = data.get('device')
    nfctagID = data.get('nfctagID')
    
    logger.info(f"Received POST on /mqtt/location, topic: {topic} from device: {device}, with nfctagID: {nfctagID}")
    
    poiID = find_poi(nfctagID)
    logger.info(f"found poiID: {poiID}, from nfctag: {nfctagID}")

    if poiID:
        socketio.emit("getlocation", {'poiID': poiID})
        return jsonify({"status": "success"}), 200
    else:
        return jsonify({"status": "error in sending poi"}), 404


#def getdata():
#    logger.info(f"returning mqttdata: {mqttdata}")
#    return mqttdata

@socketio.on('connect')
def log_socket_connect():
    logger.info("Socket connected")

