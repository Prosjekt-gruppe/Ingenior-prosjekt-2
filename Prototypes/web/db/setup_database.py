from tinydb import TinyDB
import os

base_dir = os.path.dirname(os.path.abspath(__file__))
dbpath = os.path.join(base_dir, "nfctags.json")

if not os.path.exists(dbpath):
    os.makedirs(os.path.dirname(dbpath), exist_ok=True)
    db = TinyDB(dbpath)
    db.insert({"nfctagID": "nfctag1", "poiID": 303578})
    db.insert({"nfctagID": "nfctag2", "poiID": 36148})
else:
    pass

