from tinydb import TinyDB
import os

base_dir = os.path.dirname(os.path.abspath(__file__))
nfctag_db_path = os.path.join(base_dir, "nfctags.json")


if not os.path.exists(nfctag_db_path):
    os.makedirs(os.path.dirname(nfctag_db_path), exist_ok=True)
    db = TinyDB(nfctag_db_path)
    db.insert({"nfctagID": "nfctag1", "poiID": 303578, "audio_filename": "someplace.opus"})
    db.insert({"nfctagID": "33F55B03", "poiID": 36148, "audio_filename": "bober.opus"})
    db.insert({"nfctagID": "83C7E411", "poiID": 1000459343, "audio_filename": "tapirbygget.opus"})
    db.insert({"nfctagID": "93C0930D", "poiID": 1000319015, "audio_filename": "sentralbygget.opus"})
else:
    print("nfctagdb already exists")

#https://use.mazemap.com/#v=1&campusid=1&zlevel=2&center=10.405620,63.418025&zoom=19&sharepoitype=poi&sharepoi=1000459343 tapirbygget
#https://use.mazemap.com/#v=1&campusid=1&zlevel=3&center=10.405620,63.418025&zoom=19&sharepoitype=poi&sharepoi=36148 bober
#https://use.mazemap.com/#v=1&campusid=1&zlevel=1&center=10.403742,63.417721&zoom=19.6&sharepoitype=poi&sharepoi=1000319015 sentralbygget

uuid_db_path = os.path.join(base_dir, "uuids.json")

if not os.path.exists(uuid_db_path):
    print("creating uuid-db")
    os.makedirs(os.path.dirname(uuid_db_path), exist_ok=True)
    db = TinyDB(uuid_db_path)
    r, g, b = 128, 184, 180
    db.insert({"uuid": "123e4567-e89b-12d3-a456-426614174000", "color": f"#{r:02x}{g:02x}{b:02x}"})
    r, g, b = 134, 181, 124
    db.insert({"uuid": "123e4567-e89b-12d3-a456-426614174002", "color": f"#{r:02x}{g:02x}{b:02x}"})
else:
    print("uuiddatabase already exists")

