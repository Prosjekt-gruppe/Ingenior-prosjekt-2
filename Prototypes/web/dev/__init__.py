from app import create_app

def local_dev():
    app = create_app()
    app.config['DEBUG'] = True
    return app