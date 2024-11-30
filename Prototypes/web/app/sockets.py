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

db = TinyDB('db/nfctags.json')

audiopath = "/srv/audio/"

def find_audio_file_path(poiID):
    if not isinstance(poiID, int):
        logger.info("cant find file because it search term not int")
        return

    res = db.search(Query().poiID == poiID)

    logger.info(f"from find_audio_filename: got {res}")

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
    logger.info("Starting audio stream")

    poiId = int(data.get('poiId'))

    audio_file_path = find_audio_file_path(poiId)

    #if not isinstance(audio_file_path, str):
    #    logger.info("not a string received")
    #    return socketio.emit('error', {'message': 'did not receive string'})

    if not audio_file_path.strip():
        logger.info("empty audio_file_path!")
        return socketio.emit('error', {'message': 'no audio_file_path given'})

    audiostream = AudioStream(audio_file_path)

    for chunk in audiostream.stream_chunks():
        try:
            socketio.emit('audio_chunk', chunk)
        except Exception as e:
            logger.error(f"audio stream fail, error message: {e}")
            break

    logger.info("audio stream ended")

@socketio.on('disconnect')
def handle_disconnect():
    logger.info("client disconnected")