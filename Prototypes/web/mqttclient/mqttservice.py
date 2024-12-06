import paho.mqtt.client as paho
import platform
import paho.mqtt.publish as publish
import requests
import socketio
from dotenv import load_dotenv
import os
import logging
import traceback
import json

base_dir = os.path.dirname(os.path.abspath(__file__))
dotenv_file = os.path.join(base_dir, "../.env")

load_dotenv(dotenv_file)


if platform.system() == "Linux":
    log_dir = "/var/log/pymqtt"
else:
    log_dir = os.path.join(base_dir, "../logs")


os.makedirs(log_dir, exist_ok=True)
logging.basicConfig(level=logging.INFO, filename=f"{log_dir}/mqtt.log",filemode="w")
#logging.basicConfig(level=logging.DEBUG, filename=f"{log_dir}/debug.log",filemode="w")
logger = logging.getLogger(__name__)

# Henter innloggingsdata for serverside MQTT-klient fra .env-fil 
username = os.getenv("MQTT_ADMIN_USERNAME")
password = os.getenv("MQTT_ADMIN_PASSWORD")

logger.info(f"got username {username}")

# TODO: implement database with known ids?
deviceIDs = ['1','2','3']

sio = socketio.Client()


@sio.event
def connect():
    """
    Loggfører tilkobling til socket.

    """
    logger.info("SocketIO from mqtt-client connected")

@sio.event
def disconnect():
    """
    Loggfører avkobling til socket.

    """
    logger.info("SocketIO from mqtt-client disconnected")

@sio.event
def connect_error(data):
    """
    Sier i fra om koblingen feiler.

    """
    logger.info("Failed to connect to Flask Socket.IO server")

@sio.on("strength")
def handle_strength(data):
    """
    Returnerer styrke-koeffisient til mikrokontrolleren.

    """
    logger.info("handle_strength triggered.")

    deviceid = data.get('device')

    if not deviceid:
        logger.info("Device ID not existing")
        return
    
    data.pop("device", None)

    #dev = "devices/2/data"
    try:
        payload = json.dumps(data)
        client.publish(f"devices/{deviceid}/data", payload=payload)
        logger.info(f"sending to {deviceid} with payload: {payload}")
    except:
        exc = traceback.print_exc()
        logger.info(f"!Sednging failed! Traceback: {exc}")

@sio.on("returnlocation")
def return_location(data):
    """
    Returnerer lokasjonsdata til MQTT-klienten.

    """
    logger.info(f"received {data}")

    # could drop this
    deviceID = data.get("deviceID")

    if not deviceID:
        logger.info("Device ID not existing")
        return
    
    data.pop("deviceID", None)

    try:
        payload = json.dumps(data)
        client.publish(f"devices/{deviceID}/location", payload=payload)
        logger.info(f"return_location send on socket with payload {payload}")
    except:
        logger.info("return_location failed")


# connect socket
#sio.connect("http://127.0.0.1:8000", transports=["websocket"])

def message_handler(client, msg, deviceID):
    """
    Sender beskjeder til flask-appen.

    """
    payload = {
        'topic': msg.topic,
        'device': deviceID,
        'message': msg.payload.decode()
    }
    
    try:
        response = requests.post("http://127.0.0.1:8000/mqtt", json=payload)
        logger.info(f"POST request to /mqtt complete with {response.status_code}")
    except requests.exceptions.RequestException as e:
        logger.info(e)

def location_handler(client, msg, deviceID):
    """
    Videresender lokasjonsinformasjon til flask-appen.

    """
    payload = {
        'nfctagID': msg.payload.decode(),
        'devID': deviceID
    }

    logger.info(f"received payload: {payload}")
    
    try:
        response = requests.post("http://127.0.0.1:8000/mqtt/location", json=payload)
        logger.info(f"POST request to /mqtt/location complete with {response.status_code}")
    except requests.exceptions.RequestException as e:
        logger.info(e)

# enkel måte å implementere flere topics
topics = {
    "message": message_handler,
    "location": location_handler
}


# connect mosquitto
def on_connect(client, userdata, flags, rc):
    """
    Subscriber til følgende topics:

    Forklaring:
        - ``devices/+`` godtar alle int
        - ``/#`` godtar alle str-topics

    """
    if rc == 0:
        logger.info("Connected to broker")
        client.subscribe('devices/+/#', qos=1)
    else:
        logger.info(f"Failed due to: {rc}")


def on_message(client, userdata, msg):
    """
    Funksjon som sender forespørsel til server om lokasjonsdata.

    """
    logger.info(f"topic: {msg.topic} qos: {str(msg.qos)} payload: {str(msg.payload)}")
    parts = msg.topic.split('/')

    deviceID = parts[1]
    logger.info(f"found device {deviceID}")

    if deviceID not in deviceIDs:
        logger.info(f"unkown device {deviceID} setting to unkown")
        deviceID = 'unkown'

    if parts[2] in topics:
        handler = topics.get(parts[2])
        
        #call handler from topic
        handler(client, msg, deviceID)
    else:    
        logger.info("{part} not in topic")


client = paho.Client(transport="websockets")
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(username, password)
client.connect("mqtt.gruppe1.tech", 9002)


if __name__ == "__main__":
    sio.connect("http://127.0.0.1:8000", transports=["websocket"])
    client.loop_forever()