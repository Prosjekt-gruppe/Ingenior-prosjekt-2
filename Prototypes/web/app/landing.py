from utils.logging import logger
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

# db
db = TinyDB('uuids.json')
UUID = Query()

# blueprints
bp = Blueprint('landing', __name__)

# send user to front
@bp.route('/')
@limiter.limit("5 per minute")
def redirect_front():
    return redirect(url_for('front.front'))


@bp.route('/another_test', methods=['GET'])
def another_test():
    try:
        socketio.emit("another_event", {"data": "from landing.py"})
        logger.info("another event landing.py")
        return jsonify({"status": "success"}), 200
    except Exception as e:
        logger.error(f"failed landing.py: {e}")
        return jsonify({"status": "error", "error": str(e)}), 500
