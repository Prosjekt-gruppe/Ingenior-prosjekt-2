import paho.mqtt.client as paho
import paho.mqtt.publish as publish
import requests
import socketio
from dotenv import load_dotenv
import os
import logging
import traceback
import json

load_dotenv('../.env')

log_dir = "/var/log/pymqtt"
os.makedirs(log_dir, exist_ok=True)
logging.basicConfig(level=logging.INFO, filename=f"{log_dir}/mqtt.log",filemode="w")
#logging.basicConfig(level=logging.DEBUG, filename=f"{log_dir}/debug.log",filemode="w")
logger = logging.getLogger(__name__)

username = os.getenv("MQTT_ADMIN_USERNAME")
password = os.getenv("MQTT_ADMIN_PASSWORD")

logger.info(f"got username {username}")

# TODO: implement database with known ids?
deviceIDs = ['1','2','3']

sio = socketio.Client()



@sio.event
def connect():
    logger.info("SocketIO from mqtt-client connected")

@sio.event
def disconnect():
    logger.info("SocketIO from mqtt-client disconnected")

@sio.event
def connect_error(data):
    logger.info("Failed to connect to Flask Socket.IO server")

@sio.on("strength")
def handle_strength(data):
    logger.info("handle_strength triggered.")
    #publish.single("devices/2/data", f"{data}", hostname="mqtt.eclipseprojects.io")
    dev = "devices/2/data"
    try:
        logger.info(f"sending to {dev} with data: {data}")
        client.publish(dev, f"{data}")
    except:
        exc = traceback.print_exc()
        logger.info(f"!Sednging failed! Traceback: {exc}")

@sio.on("returnlocation")
def return_location(data):
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
sio.connect("http://127.0.0.1:8000", transports=["websocket"])

def message_handler(client, msg, deviceID):
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
    payload = {
        'nfctagID': msg.payload.decode(),
        'devID': deviceID
    }
    
    try:
        response = requests.post("http://127.0.0.1:8000/mqtt/location", json=payload)
        logger.info(f"POST request to /mqtt/location complete with {response.status_code}")
    except requests.exceptions.RequestException as e:
        logger.info(e)

# topic routing table
topics = {
    "message": message_handler,
    "location": location_handler
}


# connect mosquitto
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        logger.info("Connected to broker")
        client.subscribe('devices/+/#', qos=1)
    else:
        logger.info(f"Failed due to: {rc}")


def on_message(client, userdata, msg):
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
client.loop_forever()