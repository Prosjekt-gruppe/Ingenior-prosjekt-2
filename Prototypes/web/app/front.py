import functools

from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for
)

from .mqtt import getdata

bp = Blueprint('front', __name__, url_prefix='/front')

@bp.route('/', methods=['GET'])
def front():
    mqttdata = getdata()
    return render_template('front/fpage.html', mqttdata=mqttdata)