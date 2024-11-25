from tinydb import TinyDB
import os

db = "nfctags.json"

if not os.path.exists(db):
    os.makedirs(os.path.dirname(db), exist_ok=True)
    db = TinyDB(db)
    db.insert({"nfctagID": "nfctag1", "poiID": 303578})
    db.insert({"nfctagID": "nfctag2", "poiID": 36148})
else:
    pass

