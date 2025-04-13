#pragma once

#include <arg.hpp>
#include <wl/wayland.hpp>
#include <x11/key.hpp>
#include <majorna.hpp>
#include <functional>

#if CONFIG
#if X11
struct KeyList {
    char* key{};
    KeySym keysym{};
};
#endif

#if WAYLAND
struct WlKeyList {
    char* key{};
    xkb_keysym_t keysym{};
};
#endif

#if X11
struct ModList {
    char* mod{};
    unsigned int modifier{};
};
#endif

#if WAYLAND
struct WlModList {
    char* mod{};
    char* modifier{};
};
#endif

struct ArgList {
    char* argument{};
    Arg arg{};
};

struct FuncList {
    std::string function{};
    std::function<void(Arg&)> func{};
};

inline FuncList fl[] = {
    { "moveup",           moveup },
    { "movedown",         movedown },
    { "moveleft",         moveleft },
    { "moveright",        moveright },
    { "moveend",          moveend },
    { "movestart",        movestart },
    { "movenext",         movenext },
    { "moveprev",         moveprev },
    { "moveitem",         moveitem },
    { "paste",            paste },
    { "restoresel",       restoresel },
    { "clear",            clear },
    { "clearins",         clearins },
    { "viewhist",         viewhist },
    { "moveword",         moveword },
    { "deleteword",       deleteword },
    { "movecursor",       movecursor },
    { "navhistory",       navhistory },
    { "backspace",        backspace },
    { "selectitem",       selectitem },
    { "markitem",         markitem },
    { "quit",             quit },
    { "complete",         complete },
    { "setimgsize",       setimgsize },
    { "defaultimg",       defaultimg },
    { "flipimg",          flipimg },
    { "setimgpos",        setimgpos },
    { "setimggaps",       setimggaps },
    { "setlines",         setlines },
    { "screenshot",       screenshot },
    { "setcolumns",       setcolumns },
    { "setx",             setx },
    { "sety",             sety },
    { "setw",             setw },
    { "toggleinput",      toggleinput },
    { "togglepretext",    togglepretext },
    { "togglelarrow",     togglelarrow },
    { "togglerarrow",     togglerarrow },
    { "toggleitem",       toggleitem },
    { "toggleprompt",     toggleprompt },
    { "togglecaps",       togglecaps },
    { "togglepowerline",  togglepowerline },
    { "togglecaret",      togglecaret },
    { "togglehighlight",  togglehighlight },
    { "togglematchcount", togglematchcount },
    { "togglemode",       togglemode },
    { "toggleregex",      toggleregex },
    { "togglefuzzy",      togglefuzzy },
    { "toggleimg",        toggleimg },
    { "toggleimgtype",    toggleimgtype },
    { "setprofile",       setprofile },
    { "setlineheight",    setlineheight },
    { "switchmode",       switchmode },
    { "outputhover",      outputhover },
    { "selecthover",      selecthover },
    { "markhover",        markhover },
    { "spawn",            spawn },
};

// list of modifiers that can be used in the config file
#if X11
inline ModList ml[] = {
    { "Ctrl+Shift", ShiftMask|ControlMask },
    { "Ctrl+Shift+Super", ShiftMask|ControlMask|Mod4Mask },
    { "Ctrl+Super", Mod4Mask|ControlMask },
    { "Ctrl+Alt", ControlMask|Mod1Mask },
    { "Ctrl+Alt+Shift", ShiftMask|Mod1Mask|ControlMask },
    { "Ctrl+Alt+Shift+Super", ControlMask|Mod1Mask|ShiftMask|Mod4Mask },
    { "Ctrl+Alt+Super", ControlMask|Mod1Mask|Mod4Mask },
    { "Alt+Shift", Mod1Mask|ShiftMask },
    { "Shift",   ShiftMask },
    { "Ctrl",    ControlMask },
    { "Alt",     Mod1Mask },
    { "AltGr",   Mod3Mask },
    { "Super",   Mod4Mask },
    { "ShiftGr", Mod5Mask },
    { "None",    0 },
    { "0",       0 },
};
#endif

#if WAYLAND
inline WlModList wml[] = {
    { "Ctrl+Shift", WL_CtrlShift },
    { "Ctrl+Shift+Super", WL_CtrlShiftSuper },
    { "Ctrl+Super", WL_CtrlSuper },
    { "Ctrl+Alt", WL_CtrlAlt },
    { "Ctrl+Alt+Shift", WL_CtrlAltShift },
    { "Ctrl+Alt+Shift+Super", WL_CtrlAltShiftSuper },
    { "Ctrl+Alt+Super", WL_CtrlAltSuper },
    { "Alt+Shift", WL_AltShift },
    { "Shift",   WL_Shift },
    { "Ctrl",    WL_Ctrl },
    { "Alt",     WL_Alt },
    { "Super",   WL_Super },
    { "None",    WL_None },
    { "0",       WL_None },
};
#endif

// list of keys that can be used in the config file
// expand this array if you want more
#if X11
inline KeyList kl[] = {
    { "None",       0 },
    { "Space",      XK_space },
    { "Enter",      XK_Return },
    { "Tab",        XK_Tab },
    { "a",          XK_a },
    { "b",          XK_b },
    { "c",          XK_c },
    { "d",          XK_d },
    { "e",          XK_e },
    { "f",          XK_f },
    { "g",          XK_g },
    { "h",          XK_h },
    { "i",          XK_i },
    { "j",          XK_j },
    { "k",          XK_k },
    { "l",          XK_l },
    { "m",          XK_m },
    { "n",          XK_n },
    { "o",          XK_o },
    { "p",          XK_p },
    { "q",          XK_q },
    { "r",          XK_r },
    { "s",          XK_s },
    { "t",          XK_t },
    { "u",          XK_u },
    { "v",          XK_v },
    { "w",          XK_w },
    { "x",          XK_x },
    { "y",          XK_y },
    { "z",          XK_z },
    { "0",          XK_0 },
    { "1",          XK_1 },
    { "2",          XK_2 },
    { "3",          XK_3 },
    { "4",          XK_4 },
    { "5",          XK_5 },
    { "6",          XK_6 },
    { "7",          XK_7 },
    { "8",          XK_8 },
    { "9",          XK_9 },
    { "!",          XK_exclam },
    { "\"",         XK_quotedbl },
    { "#",          XK_numbersign },
    { "$",          XK_dollar },
    { "%",          XK_percent },
    { "&",          XK_ampersand },
    { "'",          XK_apostrophe },
    { "(",          XK_parenleft },
    { ")",          XK_parenright },
    { "*",          XK_asterisk },
    { "+",          XK_plus },
    { ",",          XK_comma },
    { "-",          XK_minus },
    { ".",          XK_period },
    { "/",          XK_slash },
    { ":",          XK_colon },
    { ";",          XK_semicolon },
    { "<",          XK_less },
    { "=",          XK_equal },
    { ">",          XK_greater },
    { "?",          XK_question },
    { "@",          XK_at },
    { "[",          XK_bracketleft },
    { "\\",         XK_backslash },
    { "]",          XK_bracketright },
    { "_",          XK_underscore },
    { "grave",      XK_grave },
    { "{",          XK_braceleft },
    { "bar",        XK_bar },
    { "}",          XK_braceright },
    { "~",          XK_asciitilde },
    { "F1",         XK_F1 },
    { "F2",         XK_F2 },
    { "F3",         XK_F3 },
    { "F4",         XK_F4 },
    { "F5",         XK_F5 },
    { "F6",         XK_F6 },
    { "F7",         XK_F7 },
    { "F8",         XK_F8 },
    { "F9",         XK_F9 },
    { "F10",        XK_F10 },
    { "F11",        XK_F11 },
    { "F12",        XK_F12 },
    { "PageUp",     XK_Page_Up },
    { "PageDown",   XK_Page_Down },
    { "Home",       XK_Home },
    { "End",        XK_End },
    { "Delete",     XK_Delete },
    { "PrintScr",   XK_Print },
    { "Esc",        XK_Escape },
    { "Pause",      XK_Pause },
    { "ScrollLock", XK_Scroll_Lock },
    { "Backspace",  XK_BackSpace },
    { "Up",         XK_Up },
    { "Down",       XK_Down },
    { "Left",       XK_Left },
    { "Right",      XK_Right },
    { "Next",       XK_Next },
    { "Prior",      XK_Prior },
};
#endif

// list of keys that can be used in the config file
// expand this array if you want more
#if WAYLAND
inline WlKeyList wkl[] = {
    { "None",       0 },
    { "Space",      XKB_KEY_space },
    { "Enter",      XKB_KEY_Return },
    { "Tab",        XKB_KEY_Tab },
    { "a",          XKB_KEY_a },
    { "b",          XKB_KEY_b },
    { "c",          XKB_KEY_c },
    { "d",          XKB_KEY_d },
    { "e",          XKB_KEY_e },
    { "f",          XKB_KEY_f },
    { "g",          XKB_KEY_g },
    { "h",          XKB_KEY_h },
    { "i",          XKB_KEY_i },
    { "j",          XKB_KEY_j },
    { "k",          XKB_KEY_k },
    { "l",          XKB_KEY_l },
    { "m",          XKB_KEY_m },
    { "n",          XKB_KEY_n },
    { "o",          XKB_KEY_o },
    { "p",          XKB_KEY_p },
    { "q",          XKB_KEY_q },
    { "r",          XKB_KEY_r },
    { "s",          XKB_KEY_s },
    { "t",          XKB_KEY_t },
    { "u",          XKB_KEY_u },
    { "v",          XKB_KEY_v },
    { "w",          XKB_KEY_w },
    { "x",          XKB_KEY_x },
    { "y",          XKB_KEY_y },
    { "z",          XKB_KEY_z },
    { "0",          XKB_KEY_0 },
    { "1",          XKB_KEY_1 },
    { "2",          XKB_KEY_2 },
    { "3",          XKB_KEY_3 },
    { "4",          XKB_KEY_4 },
    { "5",          XKB_KEY_5 },
    { "6",          XKB_KEY_6 },
    { "7",          XKB_KEY_7 },
    { "8",          XKB_KEY_8 },
    { "9",          XKB_KEY_9 },
    { "!",          XKB_KEY_exclam },
    { "\"",         XKB_KEY_quotedbl },
    { "#",          XKB_KEY_numbersign },
    { "$",          XKB_KEY_dollar },
    { "%",          XKB_KEY_percent },
    { "&",          XKB_KEY_ampersand },
    { "'",          XKB_KEY_apostrophe },
    { "(",          XKB_KEY_parenleft },
    { ")",          XKB_KEY_parenright },
    { "*",          XKB_KEY_asterisk },
    { "+",          XKB_KEY_plus },
    { ",",          XKB_KEY_comma },
    { "-",          XKB_KEY_minus },
    { ".",          XKB_KEY_period },
    { "/",          XKB_KEY_slash },
    { ":",          XKB_KEY_colon },
    { ";",          XKB_KEY_semicolon },
    { "<",          XKB_KEY_less },
    { "=",          XKB_KEY_equal },
    { ">",          XKB_KEY_greater },
    { "?",          XKB_KEY_question },
    { "@",          XKB_KEY_at },
    { "[",          XKB_KEY_bracketleft },
    { "\\",         XKB_KEY_backslash },
    { "]",          XKB_KEY_bracketright },
    { "_",          XKB_KEY_underscore },
    { "grave",      XKB_KEY_grave },
    { "{",          XKB_KEY_braceleft },
    { "bar",        XKB_KEY_bar },
    { "}",          XKB_KEY_braceright },
    { "~",          XKB_KEY_asciitilde },
    { "F1",         XKB_KEY_F1 },
    { "F2",         XKB_KEY_F2 },
    { "F3",         XKB_KEY_F3 },
    { "F4",         XKB_KEY_F4 },
    { "F5",         XKB_KEY_F5 },
    { "F6",         XKB_KEY_F6 },
    { "F7",         XKB_KEY_F7 },
    { "F8",         XKB_KEY_F8 },
    { "F9",         XKB_KEY_F9 },
    { "F10",        XKB_KEY_F10 },
    { "F11",        XKB_KEY_F11 },
    { "F12",        XKB_KEY_F12 },
    { "PageUp",     XKB_KEY_Page_Up },
    { "PageDown",   XKB_KEY_Page_Down },
    { "Home",       XKB_KEY_Home },
    { "End",        XKB_KEY_End },
    { "Delete",     XKB_KEY_Delete },
    { "PrintScr",   XKB_KEY_Print },
    { "Esc",        XKB_KEY_Escape },
    { "Pause",      XKB_KEY_Pause },
    { "ScrollLock", XKB_KEY_Scroll_Lock },
    { "Backspace",  XKB_KEY_BackSpace },
    { "Up",         XKB_KEY_Up },
    { "Down",       XKB_KEY_Down },
    { "Left",       XKB_KEY_Left },
    { "Right",      XKB_KEY_Right },
    { "Next",       XKB_KEY_Next },
    { "Prior",      XKB_KEY_Prior },
};
#endif

typedef struct {
    char *click;
    unsigned int button;
} ButtonType;

typedef struct {
    char *tclick;
    unsigned int click;
} ClickType;

#if X11
inline ButtonType btp[] = {
    { "Left Click",   Button1 },
    { "Middle Click", Button2 },
    { "Right Click",  Button3 },
    { "Scroll Up",    Button4 },
    { "Scroll Down",  Button5 },
};
#endif

#if WAYLAND
inline ButtonType w_btp[] = {
    { "Left Click",   WL_Left },
    { "Middle Click", WL_Middle },
    { "Right Click",  WL_Right },
    { "Scroll Up",    WL_Up },
    { "Scroll Down",  WL_Down },
};
#endif

inline ClickType ctp[] = {
    { "ClickWindow",  ClickWindow },
    { "ClickPrompt",  ClickPrompt },
    { "ClickInput",   ClickInput },
    { "ClickLArrow",  ClickLArrow },
    { "ClickItem",    ClickItem },
    { "ClickRArrow",  ClickRArrow },
    { "ClickNumber",  ClickNumber },
    { "ClickCaps",    ClickCaps },
    { "ClickMode",    ClickMode },
#if IMAGE
    { "ClickImage",   ClickImage },
#endif
    { "None",         ClickNone },
};

void conf_init();
void theme_load();
int bind_init();
#endif
