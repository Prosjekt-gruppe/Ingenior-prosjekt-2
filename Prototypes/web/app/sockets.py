import os
from app import socketio
from utils.logging import logger
import json
import random
import uuid
from app.audiostream import AudioStream
from tinydb import TinyDB, Query

from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify, make_response
)

base_dir = os.path.dirname(os.path.abspath(__file__))
db_dir = os.path.join(base_dir, "../db")
nfctag_db = os.path.join(db_dir, "nfctags.json")
db = TinyDB(nfctag_db)

audiopath = os.path.join(base_dir, "../audio")


def find_audio_file_path(poiID):
    """
    Funksjon som finner riktig lydfil basert på tilsent interessepunkt.

    """
    if not isinstance(poiID, int):
        logger.info("cant find file because it search term not int")
        return

    res = db.search(Query().poiID == poiID)

    logger.info("from find_audio_filename: got {}".format(res[0]["audio_filename"]))
    if res:
        audio_file_path = os.path.join(audiopath, res[0]["audio_filename"])
        return audio_file_path
    else:
        logger.info(f"error from find_audio_file got res: {res} ")
        return None

@socketio.on('connect')
def handle_connect():
    logger.info("Client connected")
    socketio.emit('server_message', {'data': 'Welcome!'})

@socketio.on('ready_for_message')
def check_cookie():
    cookie = request.cookies.get("user_data")
    
    if not cookie:
        logger.info(f"No cookie found")
        socketio.emit('no_cookie', {'message': 'no cookie exists'})
        return
    
    user_data = json.loads(cookie)
    
    try:
        logger.info("sending from landing page socket")
        socketio.emit("colorchange", {"color": user_data['color']})
    except Exception as e:
        logger.error(f"failed to emit socket {e}")


@socketio.on('request_audio')
def handle_audio_request(data):
    logger.info("initializing audio request")

    poiId = int(data.get('poiId'))

    audio_file_path = find_audio_file_path(poiId)

    #if not isinstance(audio_file_path, str):
    #    logger.info("not a string received")
    #    return socketio.emit('error', {'message': 'did not receive string'})

    if not audio_file_path.strip():
        logger.info("empty audio_file_path!")
        return socketio.emit('error', {'message': 'no audio_file_path given'})

    audiostream = AudioStream(audio_file_path)
    
    logger.info(f"Started audio stream with filepath: {audio_file_path}")

    try:
        audiofile = audiostream.simple_send()
        socketio.emit('audio_file', audiofile)
    except Exception as e:
        logger.error(f"audio stream fail, error message: {e}")
        return socketio.emit('error', {'message:': 'failed to send audio'})

    logger.info("audio stream ended")

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("client disconnected")