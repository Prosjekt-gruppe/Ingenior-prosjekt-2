import functools
from utils.logging import logger
from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for, jsonify
)

bp = Blueprint('landing', __name__)

@bp.route('/')
def redirect_front():
    return redirect(url_for('front.front'))

