import paho.mqtt.client as paho
import requests
from dotenv import load_dotenv
import os
import logging

load_dotenv('../.env')

log_dir = "/var/log/pymqtt"
os.makedirs(log_dir, exist_ok=True)
logging.basicConfig(level=logging.INFO, filename=f"{log_dir}/mqtt.log",filemode="w")
logger = logging.getLogger(__name__)

username = os.getenv("MQTT_ADMIN_USERNAME")
password = os.getenv("MQTT_ADMIN_PASSWORD")

logger.info(f"got username {username}")

def on_subscribe(client, userdata, flags, rc):
    if rc == 0:
        logger.info("Connected to broker")
        client.subscribe('data', qos=1)
    else:
        logger.error(f"Failed due to: {rc}")


def on_message(client, userdata, msg):
    logger.info(msg.topic+" "+str(msg.qos)+" "+str(msg.payload)) 
    payload = {
        'topic': msg.topic,
        'message': msg.payload.decode()
    }
    try:
        requests.post("http://127.0.0.1:8000/mqtt", json=payload)
    except requests.exceptions.RequestException as e:
        logger.error(e)

client = paho.Client()
client.on_connect = on_subscribe
client.on_message = on_message
client.username_pw_set(username, password)
client.connect("mqtt.gruppe1.tech", 9001)
client.subscribe('data', qos=1)
client.loop_forever()