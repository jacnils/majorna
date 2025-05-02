from dataclasses import dataclass, field, asdict, is_dataclass
from typing import List, Any
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
class ItemTransparency:
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
class InputTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class PretextTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class PromptTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class LeftArrowTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class RightArrowTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class HighlightTransparency:
    normal_foreground: int = 255
    normal_background: int = 222
    selected_foreground: int = 255
    selected_background: int = 222

@dataclass
class MatchCounterTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class ModeIndicatorTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class CapsLockIndicatorTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class CaretTransparency:
    foreground: int = 255
    background: int = 222

@dataclass
class GeneralTransparency:
    menu: int = 222
    border: int = 222

@dataclass
class ColorItem:
    normal_foreground: str = "#bbbbbb"
    normal_background: str = "#110f1f"
    normal_foreground_second: str = "#bbbbbb"
    normal_background_second: str = "#110f1f"
    selected_foreground: str = "#110f1f"
    selected_background: str = "#8e93c2"
    selected_foreground_second: str = "#110f1f"
    selected_background_second: str = "#8e93c2"
    marked_foreground: str = "#110f1f"
    marked_background: str = "#8e93c2"
    normal_foreground_priority: str = "#bbbbbb"
    normal_background_priority: str = "#110f1f"
    selected_foreground_priority: str = "#110f1f"
    selected_background_priority: str = "#8e93c2"

@dataclass
class InputColor:
    foreground: str = "#eeeeee"
    background: str = "#110f1f"

@dataclass
class PretextColor:
    foreground: str = "#999888"
    background: str = "#110f1f"

@dataclass
class PromptColor:
    foreground: str = "#110f1f"
    background: str = "#c66e5b"

@dataclass
class LeftArrowColor:
    foreground: str = "#bbbbbb"
    background: str = "#110f1f"

@dataclass
class RightArrowColor:
    foreground: str = "#bbbbbb"
    background: str = "#110f1f"

@dataclass
class HighlightColor:
    normal_foreground: str = "#ffffff"
    normal_background: str = "#000000"
    selected_foreground: str = "#ffffff"
    selected_background: str = "#000000"

@dataclass
class MatchCounterColor:
    foreground: str = "#110f1f"
    background: str = "#eba62a"

@dataclass
class ModeIndicatorColor:
    foreground: str = "#110f1f"
    background: str = "#92c94b"

@dataclass
class CapsLockIndicatorColor:
    foreground: str = "#110f1f"
    background: str = "#f8d3e6"

@dataclass
class CaretColor:
    foreground: str = "#ffffff"
    background: str = "#110f1f"

@dataclass
class GeneralColor:
    menu: str = "#110f1f"
    border: str = "#8e93c2"

@dataclass
class AppearanceColor:
    item: ColorItem = field(default_factory=ColorItem)
    input: InputColor = field(default_factory=InputColor)
    pretext: PretextColor = field(default_factory=PretextColor)
    prompt: PromptColor = field(default_factory=PromptColor)
    left_arrow: LeftArrowColor = field(default_factory=LeftArrowColor)
    right_arrow: RightArrowColor = field(default_factory=RightArrowColor)
    highlighting: HighlightColor = field(default_factory=HighlightColor)
    match_counter: MatchCounterColor = field(default_factory=MatchCounterColor)
    mode_indicator: ModeIndicatorColor = field(default_factory=ModeIndicatorColor)
    caps_lock_indicator: CapsLockIndicatorColor = field(default_factory=CapsLockIndicatorColor)
    caret: CaretColor = field(default_factory=CaretColor)
    general: GeneralColor = field(default_factory=GeneralColor)
    sgr: List[str] = field(default_factory=lambda: [
        "#20201d", "#d73737", "#60ac39", "#cfb017", "#6684e1", "#b854d4", "#1fad83", "#a6a28c",
        "#7d7a68", "#d73737", "#60ac39", "#cfb017", "#6684e1", "#b854d4", "#1fad83", "#fefbec"
    ])

@dataclass
class Transparency:
    item: ItemTransparency = field(default_factory=ItemTransparency)
    input: InputTransparency = field(default_factory=InputTransparency)
    pretext: PretextTransparency = field(default_factory=PretextTransparency)
    prompt: PromptTransparency = field(default_factory=PromptTransparency)
    left_arrow: LeftArrowTransparency = field(default_factory=LeftArrowTransparency)
    right_arrow: RightArrowTransparency = field(default_factory=RightArrowTransparency)
    highlighting: HighlightTransparency = field(default_factory=HighlightTransparency)
    match_counter: MatchCounterTransparency = field(default_factory=MatchCounterTransparency)
    mode_indicator: ModeIndicatorTransparency = field(default_factory=ModeIndicatorTransparency)
    caps_lock_indicator: CapsLockIndicatorTransparency = field(default_factory=CapsLockIndicatorTransparency)
    caret: CaretTransparency = field(default_factory=CaretTransparency)
    general: GeneralTransparency = field(default_factory=GeneralTransparency)

@dataclass
class PowerlineStyling:
    prompt_style: PowerlineStyle = PowerlineStyle.HalfCircle
    match_counter_style: PowerlineStyle = PowerlineStyle.HalfCircle
    mode_indicator_style: PowerlineStyle = PowerlineStyle.HalfCircle
    caps_lock_indicator_style: PowerlineStyle = PowerlineStyle.HalfCircle
    item_style: PowerlineStyle = PowerlineStyle.HalfCircle

@dataclass
class PowerlineEnable:
    prompt: bool = True
    match_counter: bool = True
    mode_indicator: bool = True
    caps_lock_indicator: bool = True
    item: bool = True

@dataclass
class Hide:
    input: bool = False
    pretext: bool = False
    prompt: bool = False
    left_arrow: bool = False
    right_arrow: bool = False
    items: bool = False
    powerline: bool = False
    caret: bool = False
    highlighting: bool = False
    match_counter: bool = False
    mode_indicator: bool = False
    caps_lock_indicator: bool = False
    images: bool = False

@dataclass
class Powerline:
    style: PowerlineStyling = field(default_factory=PowerlineStyling)
    enable: PowerlineEnable = field(default_factory=PowerlineEnable)

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
class Input:
    width: float = 0.3

@dataclass
class Appearance:
    color: AppearanceColor = field(default_factory=AppearanceColor)
    transparency: Transparency = field(default_factory=Transparency)
    hide: Hide = field(default_factory=Hide)
    powerline: Powerline = field(default_factory=Powerline)
    line: Line = field(default_factory=Line)
    input: Input = field(default_factory=Input)
    caret: Caret = field(default_factory=Caret)
    color_items: bool = True
    enable_sgr_sequences: bool = True

@dataclass
class Display:
    sort: bool = True
    preselected: int = 0

@dataclass
class Matching:
    case_sensitive: bool = False
    fuzzy_matching: bool = True
    regex_matching: bool = False

@dataclass
class Permissions:
    mark: bool = True
    typing: bool = True
    password: bool = False

@dataclass
class Output:
    print_index: bool = False
    incremental: bool = False

@dataclass
class Miscellaneous:
    fast: bool = True
    delimiters: str = " /?\\\"&[]"
    list_file: str = ""
    start_mode: Mode = Mode.Normal
    force_insert_mode: bool = True
    scroll_distance: int = 512

@dataclass
class Pango:
    item: bool = True
    prompt: bool = True
    input: bool = True
    pretext: bool = True
    left_arrow: bool = False
    right_arrow: bool = False
    match_counter: bool = False
    mode_indicator: bool = False
    caps_lock_indicator: bool = False
    password: bool = False

@dataclass
class Behavior:
    display: Display = field(default_factory=Display)
    matching: Matching = field(default_factory=Matching)
    permissions: Permissions = field(default_factory=Permissions)
    output: Output = field(default_factory=Output)
    miscellaneous: Miscellaneous = field(default_factory=Miscellaneous)
    pango: Pango = field(default_factory=Pango)

@dataclass
class History:
    max_number_of_entries: int = 64
    save_duplicates: bool = False

@dataclass
class ImageDimensions:
    width: int = 200
    height: int = 200
    gaps: int = 0

@dataclass
class Image:
    dimensions: ImageDimensions = field(default_factory=ImageDimensions)
    position: ImagePosition = ImagePosition.Top
    type: ImageType = ImageType.Image
    cache: bool = True
    max_size_to_cache: int = 512
    resize_to_fit: bool = True

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
class Text:
    font: str = "Noto Sans Mono 10"
    left_arrow: str = "<"
    right_arrow: str = ">"
    password: str = "*"
    prompt: str = ""
    pretext: str = ""
    input: str = ""
    normal_mode: str = "Normal"
    insert_mode: str = "Insert"
    regex_mode: str = "Regex"
    caps_lock_on: str = "Caps Lock"
    caps_lock_off: str = ""
    padding: int = 0
    normal_item_padding: int = 0
    selected_item_padding: int = 0
    priority_item_padding: int = 0

@dataclass
class Settings:
    window: Window = field(default_factory=Window)
    x11: X11 = field(default_factory=X11)
    text: Text = field(default_factory=Text)
    appearance: Appearance = field(default_factory=Appearance)
    behavior: Behavior = field(default_factory=Behavior)
    history: History = field(default_factory=History)
    image: Image = field(default_factory=Image)
    filesystem: Filesystem = field(default_factory=Filesystem)
    keys: List[Key] = field(default_factory=lambda: [
        Key(mode=KeyMode.Any, modifier="None", key="Enter", function="select_item", argument=1),
        Key(mode=KeyMode.Any, modifier="Shift", key="Enter", function="select_item", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="Enter", function="mark_item", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Tab", function="complete", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="v", function="paste", argument=2),
        Key(mode=KeyMode.Any, modifier="None", key="Backspace", function="backspace", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="Backspace", function="delete_word", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl+Shift", key="p", function="set_profile", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="=", function="set_image_size", argument=10),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="-", function="set_image_size", argument=-10),
        Key(mode=KeyMode.Any, modifier="None", key="Up", function="move_up", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Down", function="move_down", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Left", function="move_left", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Right", function="move_right", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="u", function="move_up", argument=5),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="d", function="move_down", argument=5),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="h", function="view_history", argument=0),
        Key(mode=KeyMode.Any, modifier="None", key="Esc", function="quit", argument=0),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="p", function="navigate_history", argument=-1),
        Key(mode=KeyMode.Any, modifier="Ctrl", key="n", function="navigate_history", argument=1),
        Key(mode=KeyMode.Any, modifier="None", key="PrintScr", function="screenshot", argument=0),
    ])
    mouse: List[Mouse] = field(default_factory=lambda: [
        Mouse(click=ClickType.Input, button=ButtonType.LeftClick, function="clear"),
        Mouse(click=ClickType.Prompt, button=ButtonType.LeftClick, function="clear"),
        Mouse(click=ClickType.ModeIndicator, button=ButtonType.LeftClick, function="toggle_mode"),
        Mouse(click=ClickType.MatchCounter, button=ButtonType.LeftClick, function="view_history"),
        Mouse(click=ClickType.Item, button=ButtonType.LeftClick, function="select_hover"),
        Mouse(click=ClickType.Item, button=ButtonType.RightClick, function="mark_hover"),
        Mouse(click=ClickType.Any, button=ButtonType.ScrollUp, function="move_prev"),
        Mouse(click=ClickType.Any, button=ButtonType.ScrollDown, function="move_next"),
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
