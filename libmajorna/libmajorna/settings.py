from dataclasses import dataclass, asdict, is_dataclass
from enum import Enum
from typing import List
import json

class WindowPosition(Enum):
    Bottom = 0
    Top = 1
    Center = 2

class ItemPosition(Enum):
    Bottom = 0,
    Top = 1,

class ImagePosition(Enum):
    Top = 0,
    Bottom = 1,
    Center = 2,
    TopCenter = 3,

class ImageType(Enum):
    Icon = 0,
    Image = 1,

class PreferredProtocol(Enum):
    X11 = 0
    Wayland = 1

class PowerlineStyle(Enum):
    Arrow = 0,
    Slash = 1,
    HalfCircle = 2,

class Mode(Enum):
    Normal = 0,
    Insert = 1,

class KeyMode(Enum):
    Any = -1,
    Normal = 0,
    Insert = 1,

class ClickType(Enum):
    Window = 0,
    Prompt = 1,
    Input = 2,
    LeftArrow = 3,
    RightArrow = 4,
    Item = 5,
    MatchCounter = 6,
    CapsLockIndicator = 7,
    ModeIndicator = 8,
    Image = 9,
    Any = 10,

class ButtonType(Enum):
    LeftClick = 0,
    RightClick = 1,
    MiddleClick = 2,
    ScrollUp = 3,
    ScrollDown = 4,

@dataclass
class Dimension:
    x: int = 0
    y: int = 0
    width: int = 0


@dataclass
class CaretDimension:
    width: int = 0
    height: int = 0
    padding: int = 0


@dataclass
class PowerlineStyleSection:
    prompt_style: PowerlineStyle
    match_counter_style: PowerlineStyle
    mode_indicator_style: PowerlineStyle
    caps_lock_indicator_style: PowerlineStyle
    item_style: PowerlineStyle


@dataclass
class PowerlineEnableSection:
    prompt: bool
    match_counter: bool
    mode_indicator_style: bool
    caps_lock_indicator_style: bool
    item_style: bool


@dataclass
class PowerlineHideSection:
    input: bool
    pretext: bool
    prompt: bool
    left_arrow: bool
    right_arrow: bool
    items: bool
    powerline: bool
    caret: bool
    highlighting: bool
    match_counter: bool
    mode_indicator: bool
    caps_lock_indicator: bool
    images: bool


@dataclass
class KeyBinding:
    mode: KeyMode
    modifier: str
    key: str
    function: str
    argument: int = 0

@dataclass
class MouseBinding:
    click: ClickType
    button: ButtonType
    function: str
    argument: int = 0

@dataclass
class ImageDimensions:
    width: int
    height: int
    gaps: int
    position: ImagePosition
    type: ImageType
    cache: bool
    max_size_to_cache: int
    resize_to_fit: bool


@dataclass
class MajornaSettings:
    window: dict
    x11: dict
    text: dict
    appearance: dict
    behavior: dict
    history: dict
    image: dict
    filesystem: dict
    keys: List[KeyBinding]
    mouse: List[MouseBinding]


def get_default_settings():
    return MajornaSettings(
        window={
            "position": WindowPosition.Center,
            "monitor": -1,
            "protocol": PreferredProtocol.Wayland,
            "border_size": 0,
            "margin_vertical": 0,
            "margin_horizontal": 0,
            "padding_vertical": 0,
            "padding_horizontal": 0,
            "dimensions": {"x": 0, "y": 0, "width": 0},
            "center_width": 1000,
            "grab_input": True,
            "enable_transparency": True,
        },
        x11={
            "class": "majorna",
            "dock_type": True,
            "managed": False,
            "xresources": True,
        },
        text={
            "font": "Noto Sans Mono 10",
            "left_arrow": "<",
            "right_arrow": ">",
            "password": "*",
            "prompt": "",
            "pretext": "",
            "input": "",
            "normal_mode": "Normal",
            "insert_mode": "Insert",
            "regex_mode": "Regex",
            "caps_lock_on": "Caps Lock",
            "caps_lock_off": "",
            "padding": 0,
            "normal_item_padding": 0,
            "selected_item_padding": 0,
            "priority_item_padding": 0,
        },
        appearance={},
        behavior={
            "display": {
                "sort": True,
                "preselected": 0,
            },
            "matching": {
                "case_sensitive": False,
                "fuzzy_matching": True,
                "regex_matching": False,
            },
            "permissions": {
                "mark": True,
                "typing": True,
                "password": False,
            },
            "output": {
                "print_index": False,
                "incremental": False,
            },
            "miscellaneous": {
                "fast": True,
                "delimiters": " /?\\\\\"&[]",
                "list_file": "",
                "start_mode": Mode.Normal,
                "force_insert_mode": True,
                "scroll_distance": 512,
            },
            "pango": {
                "item": True,
                "prompt": True,
                "input": True,
                "pretext": True,
                "left_arrow": False,
                "right_arrow": False,
                "match_counter": False,
                "mode_indicator": False,
                "caps_lock_indicator": False,
                "password": False,
            },
        },
        history={
            "max_number_of_entries": 64,
            "save_duplicates": False,
        },
        image={
            "dimensions": {
                "width": 200,
                "height": 200,
                "gaps": 0,
                "position": ImagePosition.Top,
                "type": ImageType.Image,
                "cache": True,
                "max_size_to_cache": 512,
                "resize_to_fit": True,
            },
        },
        filesystem={
            "paths": {
                "fifo": "/tmp/majorna.fifo",
                "screenshot": "%h/Screenshots/majorna-%d-%t.png",
                "image_cache_directory": "default",
            }
        },
        keys=[
            KeyBinding(KeyMode.Any, "None", "Enter", "selectitem", 1),
            KeyBinding(KeyMode.Any, "Shift", "Enter", "selectitem", 0),
        ],
        mouse=[
            MouseBinding(ClickType.Input, ButtonType.LeftClick, "clear"),
            MouseBinding(ClickType.Prompt, ButtonType.LeftClick, "clear"),
        ]
    )

def settings_to_json(settings, indent=2):
    def enum_to_value(obj):
        if isinstance(obj, Enum):
            return obj.value
        elif isinstance(obj, list):
            return [enum_to_value(item) for item in obj]
        elif isinstance(obj, dict):
            return {key: enum_to_value(value) for key, value in obj.items()}
        elif is_dataclass(obj):
            return enum_to_value(asdict(obj))
        else:
            return obj

    return json.dumps(enum_to_value(settings), indent=indent)