import functools

from flask import (
    Blueprint, flash, g, redirect, render_template, request, session, url_for
)

bp = Blueprint('front', __name__, url_prefix='/front')

@bp.route('/', methods=['GET'])
def front():
    return render_template('front/fpage.html')