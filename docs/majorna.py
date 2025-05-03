# majorna.py
# ---------
# This is the default configuration file for Majorna.
# It should be placed in ~/.config/majorna/ as majorna.py.
# If it isn't being loaded on startup, ensure Python is installed as it is not a hard dependency for Majorna.
from libmajorna import settings
from libmajorna.settings import *
from libmajorna import core

# This is a Settings object with all the default options specified.
# You can get an identical object by calling settings.get_default_config()
# It is safe to remove any values you don't want to explicitly specify.
settings = Settings(
    window=Window(
        position=WindowPosition.Center, # Position to place Majorna in
        monitor=-1, # Monitor to place Majorna on. -1 = auto
        protocol=PreferredProtocol.Wayland, # Preferred protocol
        border_size=0, # Size of the border around the window
        margin_vertical=0, # Vertical margin
        margin_horizontal=0, # Horizontal margin
        padding_vertical=0, # Vertical padding
        padding_horizontal=0, # Horizontal padding
        dimensions=Dimensions( # Window positioning/dimensions, 0 = auto
            x=0,
            y=0,
            width=0
        ),
        center_width=1000, # Window width when position=WindowPosition.Center
        grab_input=True, # Grab keyboard and mouse input
        enable_transparency=True # Enable transparency (Requires a compositor on X11)
    ),
    x11=X11(
        class_name='majorna', # Class name
        dock_type=True, # Set the _NET_WM_WINDOW_TYPE_DOCK property
        managed=False, # Create the window as a standard window
        xresources=True # Enable xrdb/.Xresources support for loading properties
    ),
    text=Text(
        font='Noto Sans Mono 10', # Preferred font (syntax: <font name> <font size>). Bitmap fonts are not supported.
        left_arrow='<', # Symbol to represent the arrow on the left side, when items are present on the left.
        right_arrow='>', # Symbol to represent the arrow on the right side, when items are present on the right.
        password='*', # Symbol to use in place of each character when password mode is enabled.
        prompt='', # Default prompt text
        pretext='', # Default pretext
        input='', # Default input
        normal_mode='Normal', # Normal mode text
        insert_mode='Insert', # Insert mode text
        regex_mode='Regex', # Regex mode text
        caps_lock_on='Caps Lock', # Caps Lock on text
        caps_lock_off='', # Caps Lock off text
        padding=0, # Padding around all drawn text
        normal_item_padding=0, # Padding around normal items
        selected_item_padding=0, # Padding around selected items
        priority_item_padding=0 # Padding around high priority items
    ),
    appearance=Appearance(
        color=AppearanceColor(
            item=ColorItem(
                normal_foreground='#bbbbbb',# Normal item foreground color
                normal_background='#110f1f', # Normal item background color
                normal_foreground_second='#bbbbbb', # Normal item foreground color (every second item)
                normal_background_second='#110f1f', # Normal item background color (every second item)
                selected_foreground='#110f1f', # Selected item foreground color
                selected_background='#8e93c2', # Selected item background color
                selected_foreground_second='#110f1f', # Selected item foreground color (every second item)
                selected_background_second='#8e93c2', # Selected item background color (every second item)
                marked_foreground='#110f1f', # Marked item foreground color
                marked_background='#8e93c2', # Marked item background color
                normal_foreground_priority='#bbbbbb', # Normal, high priority foreground color
                normal_background_priority='#110f1f', # Normal, high priority background color
                selected_foreground_priority='#110f1f', # Selected, high priority, foreground color
                selected_background_priority='#8e93c2', # Selected, high priority, background color
            ),
            input=InputColor(
                foreground='#eeeeee', # Input foreground color
                background='#110f1f', # Input background color
            ),
            pretext=PretextColor(
                foreground='#999888', # Pretext foreground color
                background='#110f1f', # Pretext background color
            ),
            prompt=PromptColor(
                foreground='#110f1f', # Prompt foreground color
                background='#c66e5b', # Prompt background color
            ),
            left_arrow=LeftArrowColor(
                foreground='#bbbbbb', # Left arrow foreground color
                background='#110f1f', # Left arrow background color
            ),
            right_arrow=RightArrowColor(
                foreground='#bbbbbb', # Right arrow foreground color
                background='#110f1f', # Right arrow background color
            ),
            highlighting=HighlightColor(
                normal_foreground='#ffffff', # Normal highlighting foreground color
                normal_background='#000000', # Normal highlighting background color
                selected_foreground='#ffffff', # Selected highlighting foreground color
                selected_background='#000000', # Selected highlighting background color
            ),
            match_counter=MatchCounterColor(
                foreground='#110f1f', # Match counter foreground color
                background='#eba62a', # Match counter background color
            ),
            mode_indicator=ModeIndicatorColor(
                foreground='#110f1f', # Mode indicator foreground color
                background='#92c94b', # Mode indicator background color
            ),
            caps_lock_indicator=CapsLockIndicatorColor(
                foreground='#110f1f', # Caps Lock indicator foreground color
                background='#f8d3e6', # Caps Lock indicator background color
            ),
            caret=CaretColor(
                foreground='#ffffff', # Caret foreground color
                background='#110f1f', # Caret background color
            ),
            general=GeneralColor(
                menu='#110f1f', # Menu color
                border='#8e93c2', # Border color
            ),
            sgr=[
                '#20201d', # SGR color 0
                '#d73737', # SGR color 1
                '#60ac39', # SGR color 2
                '#cfb017', # SGR color 3
                '#6684e1', # SGR color 4
                '#b854d4', # SGR color 5
                '#1fad83', # SGR color 6
                '#a6a28c', # SGR color 7
                '#7d7a68', # SGR color 8
                '#d73737', # SGR color 9
                '#60ac39', # SGR color 10
                '#cfb017', # SGR color 11
                '#6684e1', # SGR color 12
                '#b854d4', # SGR color 13
                '#1fad83', # SGR color 14
                '#fefbec', # SGR color 15
            ]
        ),
        transparency=Transparency(
            item=ItemTransparency(
                normal_foreground=255, # Normal item foreground alpha
                normal_background=222, # Normal item background alpha
                normal_foreground_second=255, # Normal item foreground alpha (every second item)
                normal_background_second=222, # Normal item background alpha (every second item)
                selected_foreground=255, # Selected item foreground alpha
                selected_background=222, # Selected item background alpha
                selected_foreground_second=255, # Selected item foreground alpha (every second item)
                selected_background_second=222, # Selected item background alpha (every second item)
                marked_foreground=255, # Marked item foreground alpha
                marked_background=222, # Marked item background alpha
                normal_foreground_priority=255, # Normal, high priority foreground alpha
                normal_background_priority=222, # Normal, high priority background alpha
                selected_foreground_priority=255, # Selected, high priority foreground alpha
                selected_background_priority=222, # Selected, high priority background alpha
            ),
            input=InputTransparency(
                foreground=255, # Input foreground alpha
                background=222, # Input background alpha
            ),
            pretext=PretextTransparency(
                foreground=255, # Pretext foreground alpha
                background=222, # Pretext background alpha
            ),
            prompt=PromptTransparency(
                foreground=255, # Prompt foreground alpha
                background=222, # Prompt background alpha
            ),
            left_arrow=LeftArrowTransparency(
                foreground=255, # Left arrow foreground alpha
                background=222, # Left arrow background alpha
            ),
            right_arrow=RightArrowTransparency(
                foreground=255, # Right arrow foreground alpha
                background=222, # Right arrow background alpha
            ),
            highlighting=HighlightTransparency(
                normal_foreground=255, # Normal highlighting foreground alpha
                normal_background=222, # Normal highlighting background alpha
                selected_foreground=255, # Selected highlighting foreground alpha
                selected_background=222, # Selected highlighting background alpha
            ),
            match_counter=MatchCounterTransparency(
                foreground=255, # Match counter foreground alpha
                background=222, # Match counter background alpha
            ),
            mode_indicator=ModeIndicatorTransparency(
                foreground=255, # Mode indicator foreground alpha
                background=222, # Mode indicator background alpha
            ),
            caps_lock_indicator=CapsLockIndicatorTransparency(
                foreground=255, # Caps Lock indicator foreground alpha
                background=222, # Caps Lock indicator background alpha
            ),
            caret=CaretTransparency(
                foreground=255, # Caret foreground alpha
                background=222, # Caret background alpha
            ),
            general=GeneralTransparency(
                menu=222, # Menu alpha
                border=222, # Border alpha
            )
        ),
        hide=Hide(
            input=False, # Hide input
            pretext=False, # Hide pretext
            prompt=False, # Hide prompt
            left_arrow=False, # Hide left arrow
            right_arrow=False, # Hide right arrow
            items=False, # Hide items
            powerline=False, # Hide powerline
            caret=False, # Hide caret
            highlighting=False, # Hide highlighting
            match_counter=False, # Hide match counter
            mode_indicator=False, # Hide mode indicator
            caps_lock_indicator=False, # Hide caps lock indicator
            images=False, # Hide items
        ),
        powerline=Powerline(
            style=PowerlineStyling(
                prompt_style=PowerlineStyle.HalfCircle, # Prompt powerline style
                match_counter_style=PowerlineStyle.HalfCircle, # Match counter powerline style
                mode_indicator_style=PowerlineStyle.HalfCircle, # Mode indicator powerline style
                caps_lock_indicator_style=PowerlineStyle.HalfCircle, # Caps lock indicator powerline style
                item_style=PowerlineStyle.HalfCircle, # Item powerline style
            ),
            enable=PowerlineEnable(
                prompt=True, # Enable powerline for the prompt
                match_counter=True, # Enable powerline for the match counter
                mode_indicator=True, # Enable powerline for the mode indicator
                caps_lock_indicator=True, # Enable powerline for the caps lock indicator
                item=True, # Enable powerline for the items
            )
        ),
        line=Line(
            item_position=ItemPosition.Bottom, # Item positioning
            height=1, # Item height, 0 = auto
            lines=0, # Default number of lines
            columns=10, # Default number of columns
            minimum_lines=0, # Minimum number of lines allowed
            indent_items_to_prompt=False, # Indent items to prompt width when lines > 0
        ),
        input=Input(
            width=0.3, # Input area width
        ),
        caret=Caret(
            dimensions=CaretDimensions(
                width=0, # Caret width
                height=0, # Caret height
                padding=0, # Caret padding
            )
        ),
        color_items=True, # Color items
        enable_sgr_sequences=True, # Enable SGR sequences
    ),
    behavior=Behavior(
        display=Display(
            sort=True, # Sort items
            preselected=0, # Preselect an item index
        ),
        matching=Matching(
            case_sensitive=False, # Case sensitive matching
            fuzzy_matching=True, # Fuzzy finding
            regex_matching=False, # Regex matching
        ),
        permissions=Permissions(
            mark=True, # Allow marking items
            typing=True, # Allow typing
            password=False, # Hide the input and any items
        ),
        output=Output(
            print_index=False, # Print out the index instead of the actual item text
            incremental=False, # Print out on every keypress
        ),
        miscellaneous=Miscellaneous(
            fast=True, # Grab keyboard before matching
            delimiters=' /?\\"&[]', # Delimiters for word deletion
            list_file='', # File to read items from
            start_mode=Mode.Normal, # Mode to start Majorna in
            force_insert_mode=True, # Force insert mode (i.e. disable normal mode)
            scroll_distance=512, # Scroll distance required for a scroll action to count (Wayland only)
        ),
        pango=Pango(
            item=True, # Enable Pango syntax for the items
            prompt=True, # Enable Pango syntax for the prompt
            input=True, # Enable Pango syntax for the input
            pretext=True, # Enable Pango syntax for the pretext
            left_arrow=False, # Enable Pango syntax for the left arrow
            right_arrow=False, # Enable Pango syntax for the right arrow
            match_counter=False, # Enable Pango syntax for the match counter
            mode_indicator=False, # Enable Pango syntax for the mode indicator
            caps_lock_indicator=False, # Enable Pango syntax for the caps lock indicator
            password=False, # Enable Pango syntax for the password input
        )
    ),
    history=History(
        max_number_of_entries=64, # Max number of entries to save in the history file
        save_duplicates=False, # Save duplicates to the history file
    ),
    image=Image(
        dimensions=ImageDimensions(
            width=200, # Default image width
            height=200, # Default image height
            gaps=0, # Default image gaps
        ),
        position=ImagePosition.Top, # Default image position
        type=ImageType.Image, # Default image type
        cache=True, # Cache images
        max_size_to_cache=512, # Max image size to cache
        resize_to_fit=True, # Resize the image to fit
    ),
    filesystem=Filesystem(
        paths=FilesystemPaths(
            fifo='/tmp/majorna.fifo', # FIFO path (Deprecated)
            screenshot='%h/Screenshots/majorna-%d-%t.png', # Screenshot path (%h = Home directory, %d = Date, %t = Time)
            image_cache_directory='default', # Image cache directory
        )
    ),
    keys=[
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Enter',
            function='select_item',
            argument=1),
        Key(
            mode=KeyMode.Any,
            modifier='Shift',
            key='Enter',
            function='select_item',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='Enter',
            function='mark_item',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Tab',
            function='complete',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='v',
            function='paste',
            argument=2),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Backspace',
            function='backspace',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='Backspace',
            function='delete_word',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl+Shift',
            key='p',
            function='set_profile',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='=',
            function='set_image_size',
            argument=10),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='-',
            function='set_image_size',
            argument=-10),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Up',
            function='move_up',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Down',
            function='move_down',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Left',
            function='move_left',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Right',
            function='move_right',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='u',
            function='move_up',
            argument=5),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='d',
            function='move_down',
            argument=5),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='h',
            function='view_history',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='Esc',
            function='quit',
            argument=0),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='p',
            function='navigate_history',
            argument=-1),
        Key(
            mode=KeyMode.Any,
            modifier='Ctrl',
            key='n',
            function='navigate_history',
            argument=1),
        Key(
            mode=KeyMode.Any,
            modifier='None',
            key='m',
            function='screenshot',
            argument=0)
    ],

    mouse=[
        Mouse(
            click=ClickType.Input,
            button=ButtonType.LeftClick,
            function='clear',
        ),
        Mouse(
            click=ClickType.Prompt,
            button=ButtonType.LeftClick,
            function='clear',
        ),
        Mouse(
            click=ClickType.ModeIndicator,
            button=ButtonType.LeftClick,
            function='toggle_mode'
        ),
        Mouse(
            click=ClickType.MatchCounter,
            button=ButtonType.LeftClick,
            function='view_history',
        ),
        Mouse(
            click=ClickType.Item,
            button=ButtonType.LeftClick,
            function='select_hover',
        ),
        Mouse(
            click=ClickType.Item,
            button=ButtonType.RightClick,
            function='mark_hover',
        ),
        Mouse(
            click=ClickType.Any,
            button=ButtonType.ScrollUp,
            function='move_prev',
        ),
        Mouse(
            click=ClickType.Any,
            button=ButtonType.ScrollDown,
            function='move_next'
        ),
    ]
)

# Import the settings dataclass we created
core.load_settings(settings)
