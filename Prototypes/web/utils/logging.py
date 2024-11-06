import logging
import os

logdir = "/var/log/app"
os.makedirs(logdir, exist_ok=True)
logfile = os.path.join(logdir, "app.log")

logging.basicConfig(level=logging.INFO, filename=logfile, filemode="w",
                    format="%(asctime)s %(levelname)s %(message)s")


logger = logging.getLogger("Applogs")