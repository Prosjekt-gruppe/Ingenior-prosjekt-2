from utils.logging import logger
import os
import uuid
from tinydb import TinyDB, Query
import functools
import random
import json


from flask_limiter.util import get_remote_address
from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify, make_response
)

# local imports
from app import socketio, limiter

base_dir = os.path.dirname(os.path.abspath(__file__))

# db
db_uuid_file = os.path.join(base_dir, "../db/uuids.json")
db = TinyDB(db_uuid_file)

UUID = Query()

# blueprints
bp = Blueprint('landing', __name__)

# send user to front
@bp.route('/')
@limiter.limit("5 per minute")
def redirect_front():
    """
    Funksjon for å redirigere trafikk til landingssiden (front).

    :rute:
        GET /

    :return: Rediriger nettleseren til /front-endepunktet.
    """
    return redirect(url_for('front.front'))


@bp.route("/set_cookie")
def set_cookie():
    """
    Oppretter cookie i nettleseren.

    Egenskaper:
        - Levetid: 3600 sekunder
        - Secure: True

    :rute:
        GET /set_cookie

    :return:
        JSON:
            - Status: om ``cookie``-en ble opprettet suksessfult eller ikke
            - Statuskode: ``200``

    """
    cookie = request.cookies.get("user_data")
    if not cookie:
        new_uuid = str(uuid.uuid4())
        r, g, b = random.randint(50, 180), random.randint(50, 180), random.randint(50, 180)
        user_data = {"uuid": new_uuid, "color": f"#{r:02x}{g:02x}{b:02x}"}

        response = make_response(jsonify({"status": "new cookie set"}))
        response.set_cookie(
            'user_data',
            json.dumps(user_data),
            max_age=3600,
            path='/',
            samesite='Lax',
            secure=True
        )
        return response
    return jsonify({"status": "cookie already exists"}), 200



@bp.route('/another_test', methods=['GET'])
def another_test():
    try:
        socketio.emit("another_event", {"data": "from landing.py"})
        logger.info("another event landing.py")
        return jsonify({"status": "success"}), 200
    except Exception as e:
        logger.error(f"failed landing.py: {e}")
        return jsonify({"status": "error", "error": str(e)}), 500
