# libmajorna
# Python interface for majorna

from libmajorna import interfacing
from libmajorna import socket_path
from libmajorna import settings
from pathlib import Path
import json

# is_running:
# Check if Majorna is running or not.
def is_running():
    return Path(socket_path).exists()


