import os
from utils.logging import logger
from flask import Blueprint, request, jsonify
from app import socketio
from tinydb import TinyDB, Query

bp = Blueprint('mqtt', __name__, url_prefix='/mqtt')

base_dir = os.path.dirname(os.path.abspath(__file__))
db_dir = os.path.join(base_dir, "../db")
nfctag_db = os.path.join(db_dir, "nfctags.json")
db = TinyDB(nfctag_db)

def find_poi(nfctagID):
    """
    Hjelpefunksjon for uthenging av database-data.

    :param nfctagID: NFC-chippen sin ID
    :type nfctagID: str
    
    :return: NFC-taggen sin tilhørende POI.
    :rtype: int
    """
    res = db.search(Query().nfctagID == nfctagID)
    logger.info(f"From find_poi: got {res}")
    
    if res:
        return res[0]["poiID"]
    else:
        return None
    
@bp.route('/test', methods=['GET'])
def test_socketio():
    """
    Test-funksjon for å sjekke at tilkoblingen fungerer.

    """
    try:
        socketio.emit("test_event", {"data": "test from mqttpy"})
        logger.info("test_event from mqtt.py")
        return jsonify({"status": "success"}), 200
    except Exception as e:
        logger.error(f"failed socket test mqtt.py: {e}")
        return jsonify({"status": "error"}), 500

@bp.route('/', methods=['POST'])
def getmqtt():
    """
    Funksjon som håndterer endepunktet for MQTT-beskjeder.

    :rute:
        GET /mqtt/

    :return:
        JSON:
            - Status: om beskjeden mottas uten problemer
            - Statuskode: ``200``
        WebSocket:
            - Endepunkt: ``mqttsocket`` sender melding.
        
    """
    logger.info("Got post request")
    

    data = request.get_json()
    topic = data.get('topic')
    device = data.get('device')
    message = data.get('message')
    
    logger.info(f"Received POST on /mqtt, topic: {topic}, message: {message}")
        
    socketio.emit("mqttsocket", {'topic': topic, 'message': message, 'device': device})

    return jsonify({"status": "success"}), 200

@bp.route('/location', methods=['POST'])
def handle_locations():
    """
    Håndterer meldinger fra mikrokontrolleren og sender videre til nettleseren.    

    :rute:
        GET /mqtt/location

    :return:
        JSON:
            - Status: beskjed om suksess eller ikke
            - Statuskode: ``200`` om ingen feil, ``404`` om feil
        WebSocket:
            - Endepunkt: ``getlocation`` sender melding.
        
    """

    logger.info("Got post request")

    data = request.get_json()
    #topic = data.get('topic')
    device = data.get('devID')
    nfctagID = str(data.get('nfctagID'))
    
    logger.info(f"Received POST on /mqtt/location, from device: {device}, with nfctagID: {nfctagID}")
    
    poiID = find_poi(nfctagID)
    
    logger.info(f"found poiID: {poiID}, from nfctag: {nfctagID}")

    if poiID:
        socketio.emit("getlocation", {'poiID': poiID, 'device':device})
        return jsonify({"status": "success"}), 200
    else:
        return jsonify({"status": "error in sending poi"}), 404


@bp.route('/returndata', methods=['POST'])
def handle_returndata():
    """
    Funksjon som videresender tolker JSON-data og videresender til MQTT-klienten på serversiden.
    
    :return: 
        JSON:
            Statusrapport.
        SocketIO:
            - Endepunkt: ``returndata``
            - Lokasjonsdata til MQTT-klienten.
    
    """
    data = request.get_json()
    
    logger.info(f"Received POI data: {data}")

    try:
        socketio.emit("returnlocation", data)
        return jsonify({"status": "success", "message": "POI data received and taken care of"}), 200
    except Exception as e:
        logger.error(f"handle_returndata failed sending to returnlocation: {e}")
        return jsonify({"error": "Error in returning location data"}), 500
