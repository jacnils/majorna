# libmajorna
# Python interface for majorna

from libmajorna import interfacing
from libmajorna import socket_path
from libmajorna import settings
from dataclasses import dataclass
from pathlib import Path
import json

@dataclass
class RectangleHandle:
    key: str

@dataclass
class TextHandle:
    key: str
    x: int

@dataclass
class Rectangle:
    x: int
    y: int
    width: int
    height: int
    color: str
    alpha: int
    filled: bool = True
    invert: bool = False

@dataclass
class Text:
    x: int
    y: int
    width: int
    height: int
    text: str
    markup: bool
    foreground_color: str
    foreground_alpha: int
    background_color: str
    background_alpha: int

# is_running:
# Check if Majorna is running or not.
def is_running():
    return Path(socket_path).exists()

# load_settings:
# Send a JSON string to Majorna to load the settings.
def load_settings(s):
    string = {
        "action": "load_config",
        "config_json": settings.serialize_to_json(s)
    }
    interfacing.majorna_pipe.send_string(json.dumps(string) + "\n")

def draw_rectangle(rect: Rectangle):
    string = {
        "action": "draw_rect",
        "x": rect.x,
        "y": rect.y,
        "width": rect.width,
        "height": rect.height,
        "color": rect.color,
        "alpha": rect.alpha,
        "filled": rect.filled,
        "invert": rect.invert
    }
    data = interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")
    if data:
        response = json.loads(data)
        return RectangleHandle(key=response["key"])
    else:
        return None

def draw_text(text: Text):
    string = {
        "action": "draw_text",
        "x": text.x,
        "y": text.y,
        "width": text.width,
        "height": text.height,
        "text": text.text,
        "markup": text.markup,
        "foreground_color": text.foreground_color,
        "foreground_alpha": text.foreground_alpha,
        "background_color": text.background_color,
        "background_alpha": text.background_alpha
    }
    data = interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")
    if data:
        response = json.loads(data)
        return TextHandle(key=response["key"], x=response["new_x"])
    else:
        return None

def remove_rectangle(handle: RectangleHandle):
    string = {
        "action": "rm_rect",
        "key": handle.key
    }
    interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")

def remove_text(handle: TextHandle):
    string = {
        "action": "rm_text",
        "key": handle.key
    }
    interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")

def clear_rect():
    string = {
        "action": "clear_rect"
    }
    interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")

def clear_text():
    string = {
        "action": "clear_text"
    }
    interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")

def draw_menu():
    string = {
        "action": "draw_menu"
    }
    interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")

def set_print_standard(standard):
    string = {
        "action": "draw_menu",
        "no_print_standard": standard
    }
    interfacing.majorna_pipe.send_string_no_timeout(json.dumps(string) + "\n")