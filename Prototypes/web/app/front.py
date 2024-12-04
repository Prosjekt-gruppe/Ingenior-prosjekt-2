import functools
from utils.logging import logger
from app import socketio

from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify
)

bp = Blueprint('front', __name__, url_prefix='/front')

@bp.route('/', methods=('GET', 'POST'))
def front():
    """
    Setter opp nettsiden sin landingsside på GET-forespørsler, 
    og sender videre JSON-data i fra forespørslen til MQTT-klienten.

    :rute:
        GET, POST /front/

    :returns: render_template|request
    """

    if request.method == 'POST':
        data = request.get_json()
        #strength_value = data.get('strength')
        logger.info(f"received post request with {data}")
        socketio.emit("strength", data)
        
        
        return jsonify({"status": "success", "message": "Value for strength received by front post"}), 200
    
    return render_template('front/fpage.html')