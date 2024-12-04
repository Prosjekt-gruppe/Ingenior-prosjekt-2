import logging
import platform
import os


base_dir = os.path.dirname(os.path.abspath(__file__))

log_dir = ""

if platform.system() == "Linux":
    log_dir = "/var/log/app"
else:
    log_dir = os.path.join(base_dir, "../logs")

os.makedirs(log_dir, exist_ok=True)
logfile = os.path.join(log_dir, "app.log")

logging.basicConfig(level=logging.INFO, filename=logfile, filemode="w",
                    format="%(asctime)s %(levelname)s %(message)s")


logger = logging.getLogger("Applogs")