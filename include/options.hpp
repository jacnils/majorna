#pragma once

/* See LICENSE file for copyright and license details. */

/* majorna options */
inline char* _class                     = "majorna"; /* Class for majorna */
inline int protocol                    = 1; /* Protocol to try first (0: X11, 1: Wayland) */
inline int fast                        = 0; /* Grab keyboard first */
inline int xresources                  = 1; /* Enable .Xresources support */
inline int loadconfig                  = 1; /* Load configuration (~/.config/majorna/majorna.conf) on runtime */
inline int loadtheme                   = 1; /* Load theme (~/.config/majorna/theme.conf) on runtime */
inline int loadbinds                   = 1; /* Load keybind file (~/.config/majorna/binds.conf) on runtime */
inline int mon                         = -1; /* Monitor to run majorna on */
inline int managed                     = 0; /* Let your window manager manage majorna? */
inline int grabkeyboard                = 1; /* Grab keyboard/general input */

/* Wayland options */
inline int scrolldistance              = 512; /* Distance to scroll for a scroll action to count */

/* Config file options */
#if CONFIG
inline char *configfile                = nullptr; /* Config file path. Default is ~/.config/majorna/majorna.conf */
inline char *themefile                 = nullptr; /* Theme file path. Default is ~/.config/majorna/theme.conf */
inline char *bindsfile                 = nullptr; /* Keybind file path. Default is ~/.config/majorna/binds.conf */
#endif

/* Window options */
inline int alpha                       = 1; /* Enable alpha */
inline int menuposition                = 2; /* Position of the menu (0: Bottom, 1: Top, 2: Center */
inline int menupaddingv                = 0; /* Vertical padding inside the menu (px) */
inline int menupaddingh                = 0; /* Horizontal padding inside the menu (px) */
inline int menuwidth                   = 0; /* majorna window width */
inline int menumarginv                 = 0; /* Vertical padding around the menu */
inline int menumarginh                 = 0; /* Horizontal padding around the menu */
inline int centerwidth                 = 1000; /* Width when centered */
inline double inputwidth               = 0.3; /* Width reserved for input text, (Menu width * input width) */
inline int xpos                        = 0; /* X position to offset majorna */
inline int ypos                        = 0; /* Y position to offset majorna */

/* Powerline options */
inline int powerlineprompt             = 1; /* Enable powerline for the prompt */
inline int powerlinecount              = 1; /* Enable powerline for the match count */
inline int powerlinemode               = 1; /* Enable powerline for the mode indicator */
inline int powerlinecaps               = 1; /* Enable powerline for the caps lock indicator */
inline int powerlineitems              = 1; /* Enable powerline for the items */
inline int promptpwlstyle              = 2; /* Prompt powerline style (0: Arrow, 1: Slash, 2: Rounded) */
inline int matchcountpwlstyle          = 2; /* Match count powerline style (0: Arrow, 1: Slash, 2: Rounded) */
inline int modepwlstyle                = 2; /* Mode indicator powerline style (0: Arrow, 1: Slash, 2: Rounded) */
inline int capspwlstyle                = 2; /* Caps lock indicator powerline style (0: Arrow, 1: Slash, 2: Rounded) */
inline int itempwlstyle                = 2; /* Item powerline style (0: Arrow, 1: Slash, 2: Rounded)) */

/* Window properties */
inline int dockproperty                = 1; /* Set _NET_WM_WINDOW_TYPE_DOCK */

/* Image options */
inline int imagewidth                  = 200; /* Default image width (px) */
inline int imageheight                 = 200; /* Default image height (px) */
inline int imageresize                 = 1; /* Allow the majorna window to resize itself to fit the image (0/1) */
inline int imagegaps                   = 0; /* Image gaps */
inline int imageposition               = 0; /* Image position (0: Top, 1: Bottom, 2: Center, 3: Top center) */
inline int imagetype                   = 1; /* Image type (0: Icon, 1: Image) */
inline int generatecache               = 1; /* Generate image cache by default */
inline int maxcache                    = 512; /* Max image size to cache */
inline char *cachedir                  = "default"; /* Cache directory. Default means majorna will determine automatically */

/* FIFO options */
#if FIFO
inline char *fifofile                  = "/tmp/majorna.fifo"; /* majorna FIFO path */
#endif

/* Screenshot options */
inline char *screenshotfile            = nullptr; /* Screenshot file path. If set to nullptr, the default path will be used. */
inline char *screenshotname            = nullptr; /* Screenshot file name. If set to nullptr, the default name will be used. */
inline char *screenshotdir             = nullptr; /* Screenshot file directory. If set to nullptr, the default directory will be used. */

/* Mode options */
inline int mode                        = 0; /* Mode to start speedwm in (0: Normal mode, 1: Insert mode) */
inline int forceinsertmode             = 1; /* Force insert mode, meaning normal mode will be disabled (0/1) */
inline char *normtext                  = "Normal"; /* Text to display for normal mode */
inline char *instext                   = "Insert"; /* Text to display for insert mode */
inline char *regextext                 = "Regex"; /* Text to display for insert mode when regex is enabled */
inline char *capslockontext            = "Caps Lock"; /* Text to display for the caps lock indicator when caps lock is on */
inline char *capslockofftext           = ""; /* Text to display for the caps lock indicator when caps lock is off */

/* Window border options */
inline int borderwidth                 = 0; /* Width of the border */

/* Font options */
inline char font[]                     = "Noto Sans Mono 10"; /* Font to draw text and Pango markup with. */
inline int textpadding                 = 0; /* Global text padding */
inline int normitempadding             = 0; /* Text padding for normal items */
inline int selitempadding              = 0; /* Text padding for the selected item */
inline int priitempadding              = 0; /* Text padding for the high priority items */

/* Text options */
inline char *leftarrow                 = "<"; /* Left arrow, used to indicate you can move to the left */
inline char *rightarrow                = ">"; /* Right arrow, used to indicate you can move to the right */
inline char *password                  = "*"; /* Password character, when the -P argument is active this will replace all characters typed */
inline char *prompt                    = nullptr; /* Default prompt */
inline char *input                     = nullptr; /* Default input text */
inline char *pretext                   = nullptr; /* Default pretext */

/* Match options */
inline int type                        = 1; /* Allow typing into majorna or only allow keybinds. */
inline int passwd                      = 0; /* Replace input with another character and don't read stdin */
inline int sortmatches                 = 1; /* Sort matches (0/1) */
inline int casesensitive               = 0; /* Case-sensitive by default? (0/1) */
inline int mark                        = 1; /* Enable marking items (multi selection) (0/1) */
inline int preselected                 = 0; /* Which line should majorna preselect? */
inline int fuzzy                       = 1; /* Whether or not to enable fuzzy matching by default */
inline int regex                       = 0; /* Whether or not to enable regex matching by default */
inline char *listfile                  = nullptr; /* File to read entries from instead of stdin. nullptr means read from stdin instead. */

/* Line options */
inline int itemposition                = 0; /* Item position (0: Bottom, 1: Top) */
inline int lineheight                  = 1; /* Line height (0: Calculate automatically) */
inline int lines                       = 0; /* Default number of lines */
inline int columns                     = 10; /* Default number of columns */
inline int overridelines               = 1; /* Allow overriding lines using keybinds */
inline int overridecolumns             = 1; /* Allow overriding columns using keybinds */
inline int minlines                    = 0; /* Minimum number of lines allowed */

/* History options */
inline char *histfile                  = nullptr; /* History file, nullptr means no history file */
inline int maxhist                     = 64; /* Max number of history entries */
inline int histdup                     = 0;	/* If 1, record repeated histories */

/* Prompt options */
inline int indentitems                 = 0; /* Indent items to prompt width? (0/1) */

/* Caret options */
inline int caretwidth                  = 0; /* Caret width (0: Calculate automatically) */
inline int caretheight                 = 0; /* Caret height (0: Calculate automatically) */
inline int caretpadding                = 0; /* Caret padding (px) */

/* Hide options */
inline int hideinput                   = 0; /* Hide input (0/1) */
inline int hidepretext                 = 0; /* Hide pretext (0/1) */
inline int hidelarrow                  = 0; /* Hide left arrow (0/1) */
inline int hiderarrow                  = 0; /* Hide right arrow (0/1) */
inline int hideitem                    = 0; /* Hide item (0/1) */
inline int hideprompt                  = 0; /* Hide prompt (0/1) */
inline int hidecaps                    = 0; /* Hide caps lock indicator (0/1) */
inline int hidepowerline               = 0; /* Hide powerline (0/1) */
inline int hidecaret                   = 0; /* Hide caret (0/1) */
inline int hidehighlight               = 0; /* Hide highlight (0/1) */
inline int hidematchcount              = 0; /* Hide match count (0/1) */
inline int hidemode                    = 0; /* Hide mode (0/1) */
inline int hideimage                   = 0; /* Hide image (0/1) */

/* Color options
 *
 * Item colors */
inline char col_itemnormfg[]           = "#bbbbbb"; /* Normal foreground item color */
inline char col_itemnormbg[]           = "#110f1f"; /* Normal background item color */
inline char col_itemnormfg2[]          = "#bbbbbb"; /* Normal foreground item colors for the next item */
inline char col_itemnormbg2[]          = "#110f1f"; /* Normal background item colors for the next item */
inline char col_itemselfg[]            = "#110f1f"; /* Selected foreground item color */
inline char col_itemselbg[]            = "#8e93c2"; /* Selected background item color */
inline char col_itemmarkedfg[]         = "#110f1f"; /* Marked foreground item color */
inline char col_itemmarkedbg[]         = "#8e93c2"; /* Marked background item color */
inline char col_itemnormprifg[]        = "#bbbbbb"; /* Normal foreground item (high priority) color */
inline char col_itemnormpribg[]        = "#110f1f"; /* Normal background item (high priority) color */
inline char col_itemselprifg[]         = "#110f1f"; /* Selected foreground item (high priority) color */
inline char col_itemselpribg[]         = "#8e93c2"; /* Selected background item (high priority) color */

/* Input colors */
inline char col_inputfg[]              = "#eeeeee"; /* Input field foreground color */
inline char col_inputbg[]              = "#110f1f"; /* Input field background color */

/* Pretext colors */
inline char col_pretextfg[]            = "#999888"; /* Pretext foreground color */
inline char col_pretextbg[]            = "#110f1f"; /* Pretext background color */

/* Menu colors */
inline char col_menu[]                 = "#110f1f"; /* Menu background color */

/* Prompt colors */
inline char col_promptfg[]             = "#110f1f"; /* Prompt foreground color */
inline char col_promptbg[]             = "#c66e5b"; /* Prompt background color */

/* Arrow colors */
inline char col_larrowfg[]             = "#bbbbbb"; /* Left arrow color */
inline char col_rarrowfg[]             = "#bbbbbb"; /* Right arrow color */
inline char col_larrowbg[]             = "#110f1f"; /* Left arrow color */
inline char col_rarrowbg[]             = "#110f1f"; /* Right arrow color */

/* Normal highlight colors */
inline char col_hlnormfg[]             = "#ffffff"; /* Normal text highlight color */
inline char col_hlnormbg[]             = "#000000"; /* Normal background highlight color */

/* Selected highlight colors */
inline char col_hlselfg[]              = "#ffffff"; /* Selected text highlight color */
inline char col_hlselbg[]              = "#000000"; /* Selected background highlight color */

/* Match count colors */
inline char col_numfg[]                = "#110f1f"; /* Match count text color */
inline char col_numbg[]                = "#eba62a"; /* Match count background color */

/* Border color */
inline char col_border[]               = "#8e93c2"; /* Border color */

/* Caret colors */
inline char col_caretfg[]              = "#ffffff"; /* Foreground caret color */
inline char col_caretbg[]              = "#110f1f"; /* Background caret color */

/* Mode colors */
inline char col_modefg[]               = "#110f1f"; /* Mode text color */
inline char col_modebg[]               = "#92c94b"; /* Mode background color */

/* Caps lock colors */
inline char col_capsfg[]               = "#110f1f"; /* Caps lock text color */
inline char col_capsbg[]               = "#f8d3e6"; /* Caps lock background color */

/* SGR colors */
inline char col_sgr0[]                 = "#20201d"; /* SGR color #0 */
inline char col_sgr1[]                 = "#d73737"; /* SGR color #1 */
inline char col_sgr2[]                 = "#60ac39"; /* SGR color #2 */
inline char col_sgr3[]                 = "#cfb017"; /* SGR color #3 */
inline char col_sgr4[]                 = "#6684e1"; /* SGR color #4 */
inline char col_sgr5[]                 = "#b854d4"; /* SGR color #5 */
inline char col_sgr6[]                 = "#1fad83"; /* SGR color #6 */
inline char col_sgr7[]                 = "#a6a28c"; /* SGR color #7 */
inline char col_sgr8[]                 = "#7d7a68"; /* SGR color #8 */
inline char col_sgr9[]                 = "#d73737"; /* SGR color #9 */
inline char col_sgr10[]                = "#60ac39"; /* SGR color #10 */
inline char col_sgr11[]                = "#cfb017"; /* SGR color #11 */
inline char col_sgr12[]                = "#6684e1"; /* SGR color #12 */
inline char col_sgr13[]                = "#b854d4"; /* SGR color #13 */
inline char col_sgr14[]                = "#1fad83"; /* SGR color #14 */
inline char col_sgr15[]                = "#fefbec"; /* SGR color #15 */

/* Alpha options */
inline int alpha_itemnormfg            = 255; /* Alpha for normal item foreground (0-255) */
inline int alpha_itemnormbg            = 222; /* Alpha for normal item background (0-255) */
inline int alpha_itemnormfg2           = 255; /* Alpha for next normal item foreground (0-255) */
inline int alpha_itemnormbg2           = 222; /* Alpha for next normal item background (0-255) */
inline int alpha_itemselfg             = 255; /* Alpha for selected item foreground (0-255) */
inline int alpha_itemselbg             = 222; /* Alpha for selected item background (0-255) */
inline int alpha_itemmarkedfg          = 255; /* Alpha for marked item foreground (0-255) */
inline int alpha_itemmarkedbg          = 222; /* Alpha for marked item background (0-255) */
inline int alpha_itemnormprifg         = 255; /* alpha for normal priority item foreground (0-255) */
inline int alpha_itemnormpribg         = 222; /* Alpha for normal priority item background (0-255) */
inline int alpha_itemselprifg          = 255; /* Alpha for selected priority item foreground (0-255) */
inline int alpha_itemselpribg          = 222; /* Alpha for selected priority item background (0-255) */
inline int alpha_inputfg               = 255; /* Alpha for input foreground (0-255) */
inline int alpha_inputbg               = 222; /* Alpha for input background (0-255) */
inline int alpha_pretextfg             = 255; /* Alpha for pretext foreground (0-255) */
inline int alpha_pretextbg             = 222; /* Alpha for pretext background (0-255) */
inline int alpha_menu                  = 222; /* Alpha for menu background (0-255) */
inline int alpha_promptfg              = 255; /* Alpha for prompt foreground (0-255) */
inline int alpha_promptbg              = 222; /* Alpha for prompt background (0-255) */
inline int alpha_larrowfg              = 255; /* Alpha for left arrow foreground (0-255) */
inline int alpha_larrowbg              = 222; /* Alpha for left arrow background (0-255) */
inline int alpha_rarrowfg              = 255; /* Alpha for right arrow foreground (0-255) */
inline int alpha_rarrowbg              = 222; /* Alpha for right arrow background (0-255) */
inline int alpha_hlnormfg              = 255; /* Alpha for normal highlight foreground (0-255) */
inline int alpha_hlnormbg              = 222; /* Alpha for normal highlight background (0-255) */
inline int alpha_hlselfg               = 255; /* Alpha for selected highlight foreground (0-255) */
inline int alpha_hlselbg               = 222; /* Alpha for selected highlight background (0-255) */
inline int alpha_numfg                 = 255; /* Alpha for match count foreground (0-255) */
inline int alpha_numbg                 = 222; /* Alpha for the match count background (0-255) */
inline int alpha_border                = 255; /* Alpha for the border (0-255) */
inline int alpha_caretfg               = 255; /* Alpha for the caret foreground (0-255) */
inline int alpha_caretbg               = 222; /* Alpha for the caret background (0-255) */
inline int alpha_modefg                = 255; /* Alpha for the mode indicator foreground (0-255) */
inline int alpha_modebg                = 222; /* Alpha for the mode indicator background (0-255) */
inline int alpha_capsfg                = 255; /* Alpha for the caps lock indicator foreground (0-255) */
inline int alpha_capsbg                = 222; /* Alpha for the caps lock indicator background (0-255) */

/* Pango options */
inline int pango_item                  = 1; /* Enable support for pango markup for the items */
inline int pango_prompt                = 1; /* Enable support for pango markup for the prompt */
inline int pango_caps                  = 1; /* Enable support for pango markup for the caps lock indicator */
inline int pango_input                 = 1; /* Enable support for pango markup for the user input */
inline int pango_pretext               = 1; /* Enable support for pango markup for the pretext */
inline int pango_leftarrow             = 0; /* Enable support for pango markup for the left arrow */
inline int pango_rightarrow            = 0; /* Enable support for pango markup for the right arrow */
inline int pango_numbers               = 0; /* Enable support for pango markup for the match count */
inline int pango_mode                  = 0; /* Enable support for pango markup for the mode indicator */
inline int pango_password              = 0; /* Enable support for pango markup for the password text */

/* Misc */
inline int printindex                  = 0; /* Print index instead of the text itself (0/1) */
inline int requirematch                = 0; /* Require input text to match an item (0/1) */
inline int incremental                 = 0; /* Print text every time a key is pressed (0/1) */
inline int coloritems                  = 1; /* Color items (0/1) */
inline int sgr                         = 1; /* Support SGR sequences (0/1) */
inline char *worddelimiters            = " /?\"&[]"; /* Word delimiters used for keybinds that change words, Space is default. */
