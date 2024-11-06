import paho.mqtt.client as paho
import requests
from dotenv import load_dotenv
import os

load_dotenv()

username = os.getenv("MQTT_ADMIN_USERNAME")
password = os.getenv("MQTT_ADMIN_PASSWORD")


def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_message(client, userdata, msg):
    #print(msg.topic+" "+str(msg.qos)+" "+str(msg.payload)) 
    payload = {
        'topic': msg.topic,
        'message': msg.payload.decode()
    }
    try:
        requests.post("127.0.0.1:8000/mqtt", json=payload)
    except requests.exceptions.RequestException as e:
        print(e)

client = paho.Client()
client.on_connect = on_subscribe
client.on_message = on_message
client.username_pw_set(username, password)
client.connect("mqtt.gruppe1.tech", 9001)
client.subscribe('encyclopedia/#', qos=1)
client.loop_forever()