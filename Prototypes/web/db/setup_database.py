from tinydb import TinyDB
import os

base_dir = os.path.dirname(os.path.abspath(__file__))
nfctag_db_path = os.path.join(base_dir, "nfctags.json")


if not os.path.exists(nfctag_db_path):
    os.makedirs(os.path.dirname(nfctag_db_path), exist_ok=True)
    db = TinyDB(nfctag_db_path)
    db.insert({"nfctagID": "nfctag1", "poiID": 303578})
    db.insert({"nfctagID": "nfctag2", "poiID": 36148})
else:
    print("nfctagdb already exists")


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

