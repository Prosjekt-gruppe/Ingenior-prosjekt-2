import functools
from utils.logging import logger

from flask_limiter.util import get_remote_address
from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify, make_response
)

bp = Blueprint('landing', __name__)

@bp.route('/')
def redirect_front():
    user = request.cookies.get("user")

    if not user:
        response = make_response(redirect(url_for('front.front')))
        response.set_cookie('user', 'device2', max_age=3600)
        return response

    return redirect(url_for('front.front'))