from flask import Blueprint, request, jsonify
bp = Blueprint('mqtt', __name__, url_prefix='/mqtt')


mqttdata = []

@bp.route('/mqtt', methods=['POST'])
def getmqtt():
    data = request.get_json()
    topic = data.get('topic')
    message = data.get('message')
    
    mqttdata.append({'topic': topic, 'message': message})

    return jsonify({"status": "success"}), 200

def getdata():
    return mqttdata