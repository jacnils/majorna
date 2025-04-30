from dataclasses import dataclass, field, asdict, is_dataclass
from typing import List, Dict, Union, Any
from enum import Enum
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
class Dimensions:
    x: int = 0
    y: int = 0
    width: int = 0

@dataclass
class Window:
    position: WindowPosition = WindowPosition.Center
    monitor: int = -1
    protocol: PreferredProtocol = PreferredProtocol.Wayland
    border_size: int = 0
    margin_vertical: int = 0
    margin_horizontal: int = 0
    padding_vertical: int = 0
    padding_horizontal: int = 0
    dimensions: Dimensions = field(default_factory=Dimensions)
    center_width: int = 1000
    grab_input: bool = True
    enable_transparency: bool = True

@dataclass
class X11:
    class_name: str = "majorna"
    dock_type: bool = True
    managed: bool = False
    xresources: bool = True

@dataclass
class ColorItem:
    normal_foreground: str = "#bbbbbb"
    normal_background: str = "#110flf"
    normal_foreground_second: str = "#bbbbbb"
    normal_background_second: str = "#110flf"
    selected_foreground: str = "#110flf"
    selected_background: str = "#8e93c2"
    selected_foreground_second: str = "#110flf"
    selected_background_second: str = "#8e93c2"
    marked_foreground: str = "#110flf"
    marked_background: str = "#8e93c2"
    normal_foreground_priority: str = "#bbbbbb"
    normal_background_priority: str = "#110flf"
    selected_foreground_priority: str = "#110flf"
    selected_background_priority: str = "#8e93c2"

@dataclass
class TransparencyItem:
    normal_foreground: int = 255
    normal_background: int = 222
    normal_foreground_second: int = 255
    normal_background_second: int = 222
    selected_foreground: int = 255
    selected_background: int = 222
    selected_foreground_second: int = 255
    selected_background_second: int = 222
    marked_foreground: int = 255
    marked_background: int = 222
    normal_foreground_priority: int = 255
    normal_background_priority: int = 222
    selected_foreground_priority: int = 255
    selected_background_priority: int = 222

@dataclass
class AppearanceColor:
    item: ColorItem = field(default_factory=ColorItem)
    input: Dict[str, str] = field(default_factory=lambda: {"foreground": "#eeeeee", "background": "#110flf"})
    pretext: Dict[str, str] = field(default_factory=lambda: {"foreground": "#999888", "background": "#110flf"})
    prompt: Dict[str, str] = field(default_factory=lambda: {"foreground": "#110flf", "background": "#c66e5b"})
    left_arrow: Dict[str, str] = field(default_factory=lambda: {"foreground": "#bbbbbb", "background": "#110flf"})
    right_arrow: Dict[str, str] = field(default_factory=lambda: {"foreground": "#bbbbbb", "background": "#110flf"})
    highlighting: Dict[str, str] = field(default_factory=lambda: {"normal_foreground": "#ffffff", "normal_background": "#000000", "selected_foreground": "#ffffff", "selected_background": "#000000"})
    match_counter: Dict[str, str] = field(default_factory=lambda: {"foreground": "#110flf", "background": "#eba62a"})
    mode_indicator: Dict[str, str] = field(default_factory=lambda: {"foreground": "#110flf", "background": "#92c94b"})
    caps_lock_indicator: Dict[str, str] = field(default_factory=lambda: {"foreground": "#110flf", "background": "#f8d3e6"})
    caret: Dict[str, str] = field(default_factory=lambda: {"foreground": "#ffffff", "background": "#110flf"})
    general: Dict[str, str] = field(default_factory=lambda: {"menu": "#110flf", "border": "#8e93c2"})
    sgr: List[str] = field(default_factory=lambda: [
        "#20201d", "#d73737", "#60ac39", "#cfb017", "#6684e1", "#b854d4", "#1fad83", "#a6a28c",
        "#7d7a68", "#d73737", "#60ac39", "#cfb017", "#6684e1", "#b854d4", "#1fad83", "#fefbec"
    ])

@dataclass
class Transparency:
    item: TransparencyItem = field(default_factory=TransparencyItem)
    input: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    pretext: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    prompt: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    left_arrow: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    right_arrow: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    highlighting: Dict[str, int] = field(default_factory=lambda: {"normal_foreground": 255, "normal_background": 222, "selected_foreground": 255, "selected_background": 222})
    match_counter: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    mode_indicator: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    caps_lock_indicator: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    caret: Dict[str, int] = field(default_factory=lambda: {"foreground": 255, "background": 222})
    general: Dict[str, int] = field(default_factory=lambda: {"menu": 222, "border": 255})

@dataclass
class PowerlineStyling:
    prompt_style: PowerlineStyle = PowerlineStyle.HalfCircle
    match_counter_style: PowerlineStyle = PowerlineStyle.HalfCircle
    mode_indicator_style: PowerlineStyle = PowerlineStyle.HalfCircle
    caps_lock_indicator_style: PowerlineStyle = PowerlineStyle.HalfCircle
    item_style: PowerlineStyle = PowerlineStyle.HalfCircle

@dataclass
class Powerline:
    style: PowerlineStyling = field(default_factory=PowerlineStyling)
    enable: Dict[str, bool] = field(default_factory=lambda: {
        "prompt": True, "match_counter": True, "mode_indicator_style": True,
        "caps_lock_indicator_style": True, "item_style": True
    })
    hide: Dict[str, bool] = field(default_factory=lambda: {
        "input": False, "pretext": False, "prompt": False, "left_arrow": False, "right_arrow": False,
        "items": False, "powerline": False, "caret": False, "highlighting": False, "match_counter": False,
        "mode_indicator": False, "caps_lock_indicator": False, "images": False
    })

@dataclass
class Line:
    item_position: ItemPosition = ItemPosition.Bottom
    height: int = 1
    lines: int = 0
    columns: int = 10
    minimum_lines: int = 0
    indent_items_to_prompt: bool = False

@dataclass
class CaretDimensions:
    width: int = 0
    height: int = 0
    padding: int = 0

@dataclass
class Caret:
    dimensions: CaretDimensions = field(default_factory=CaretDimensions)

@dataclass
class Appearance:
    color: AppearanceColor = field(default_factory=AppearanceColor)
    transparency: Transparency = field(default_factory=Transparency)
    powerline: Powerline = field(default_factory=Powerline)
    line: Line = field(default_factory=Line)
    input: Dict[str, float] = field(default_factory=lambda: {"width": 0.3})
    caret: Caret = field(default_factory=Caret)
    color_items: bool = True
    enable_sgr_sequences: bool = True

@dataclass
class Behavior:
    display: Dict[str, Union[bool, int]] = field(default_factory=lambda: {"sort": True, "preselected": 0})
    matching: Dict[str, Union[bool, str]] = field(default_factory=lambda: {
        "case_sensitive": False, "fuzzy_matching": True, "regex_matching": False
    })
    permissions: Dict[str, bool] = field(default_factory=lambda: {"mark": True, "typing": True, "password": False})
    output: Dict[str, bool] = field(default_factory=lambda: {"print_index": False, "incremental": False})
    miscellaneous: Dict[str, Union[bool, int, str]] = field(default_factory=lambda: {
        "fast": True, "delimiters": " /?\\\"&[]", "list_file": "", "start_mode": "Normal",
        "force_insert_mode": True, "scroll_distance": 512
    })
    pango: Dict[str, bool] = field(default_factory=lambda: {
        "item": True, "prompt": True, "input": True, "pretext": True, "left_arrow": False, "right_arrow": False,
        "match_counter": False, "mode_indicator": False, "caps_lock_indicator": False, "password": False
    })

@dataclass
class History:
    max_number_of_entries: int = 64
    save_duplicates: bool = False

@dataclass
class ImageDimensions:
    width: int = 200
    height: int = 200
    gaps: int = 0
    position: ImagePosition = ImagePosition.Top
    type: ImageType = ImageType.Image
    cache: bool = True
    max_size_to_cache: int = 512
    resize_to_fit: bool = True

@dataclass
class Image:
    dimensions: ImageDimensions = field(default_factory=ImageDimensions)

@dataclass
class FilesystemPaths:
    fifo: str = "/tmp/majorna.fifo"
    screenshot: str = "%h/Screenshots/majorna-%d-%t.png"
    image_cache_directory: str = "default"

@dataclass
class Filesystem:
    paths: FilesystemPaths = field(default_factory=FilesystemPaths)

@dataclass
class Key:
    mode: KeyMode
    modifier: str
    key: str
    function: str
    argument: int = 0

@dataclass
class Mouse:
    click: ClickType
    button: ButtonType
    function: str

@dataclass
class Settings:
    window: Window = field(default_factory=Window)
    x11: X11 = field(default_factory=X11)
    text: Dict[str, Union[str, int]] = field(default_factory=lambda: {
        "font": "Noto Sans Mono 10", "left_arrow": "<", "right_arrow": ">", "password": "*", "prompt": "",
        "pretext": "", "input": "", "normal_mode": "Normal", "insert_mode": "Insert", "regex_mode": "Regex",
        "caps_lock_on": "Caps Lock", "caps_lock_off": "", "padding": 0, "normal_item_padding": 0,
        "selected_item_padding": 0, "priority_item_padding": 0
    })
    appearance: Appearance = field(default_factory=Appearance)
    behavior: Behavior = field(default_factory=Behavior)
    history: History = field(default_factory=History)
    image: Image = field(default_factory=Image)
    filesystem: Filesystem = field(default_factory=Filesystem)
    keys: List[Key] = field(default_factory=lambda: [
        Key(mode=KeyMode.Any, modifier="None", key="Enter", function="selectitem", argument=1),
        Key(mode=KeyMode.Any, modifier="Shift", key="Enter", function="selectitem", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="Enter", function="markitem", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Tab", function="complete", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="v", function="paste", argument=2),
        Key(mode=KeyMode.Any, modifier="None", key="Backspace", function="backspace", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="Backspace", function="deleteword", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl+Shift", key="p", function="setprofile", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="=", function="setimgsize", argument=10),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="-", function="setimgsize", argument=-10),
        Key(mode=KeyMode.Any, modifier="None", key="Up", function="moveup", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Down", function="movedown", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Left", function="moveleft", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Right", function="moveright", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="u", function="moveup", argument=5),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="d", function="movedown", argument=5),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="h", function="viewhist", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Esc", function="quit", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="p", function="navhistory", argument=-1),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="n", function="navhistory", argument=1),
        Key(mode=KeyMode.Any, modifier="None", key="PrintScr", function="screenshot", argument=0),
    ])
    mouse: List[Mouse] = field(default_factory=lambda: [
        Mouse(click=ClickType.Input, button=ButtonType.LeftClick, function="clear"),
        Mouse(click=ClickType.Prompt, button=ButtonType.LeftClick, function="clear"),
        Mouse(click=ClickType.ModeIndicator, button=ButtonType.LeftClick, function="switchmode"),
        Mouse(click=ClickType.MatchCounter, button=ButtonType.LeftClick, function="viewhist"),
        Mouse(click=ClickType.Item, button=ButtonType.LeftClick, function="selecthover"),
        Mouse(click=ClickType.Item, button=ButtonType.RightClick, function="markhover"),
        Mouse(click=ClickType.Any, button=ButtonType.ScrollUp, function="moveprev"),
        Mouse(click=ClickType.Any, button=ButtonType.ScrollDown, function="movenext"),
    ])

def get_default_config() -> Settings:
    return Settings()

def serialize_to_json(obj: Any) -> str:
    def custom_serializer(o):
        if isinstance(o, Enum):
            return o.value
        if is_dataclass(o):
            return {key: custom_serializer(value) for key, value in asdict(o).items()}
        if isinstance(o, (list, tuple)):
            return [custom_serializer(item) for item in o]
        if isinstance(o, dict):
            return {key: custom_serializer(value) for key, value in o.items()}
        return o

    return json.dumps(custom_serializer(obj), indent=4)
