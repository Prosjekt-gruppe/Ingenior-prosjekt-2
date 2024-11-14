import functools
from app import socketio


from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for
)

from .mqtt import getdata

bp = Blueprint('front', __name__, url_prefix='/front')

@bp.route('/', methods=('GET', 'POST'))
def front():
    if request.method == 'POST':
        strength_value = request.form['strength']
        socketio.emit("strength", strength_value)


    return render_template('front/fpage.html')