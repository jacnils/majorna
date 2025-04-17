#pragma once

#include <x11/x11_libs.hpp>
#include <arg.hpp> // only for the quit fptr
#include <functional>

struct Key {
    unsigned int mode{};
    unsigned int mod{};
    KeySym keysym{};
    std::function <void(Arg&)> func{};
    Arg arg{};
};

// user friendly names for all the modifiers we're using, but there are many more
#define Ctrl ControlMask
#define Shift ShiftMask
#define Alt Mod1Mask
#define AltGr Mod3Mask
#define Super Mod4Mask
#define ShiftGr Mod5Mask

void updatenumlockmask();
void keypress_x11(XEvent& e);
void grabkeyboard_x11();
void getcapsstate();

inline Key ckeys[256];
inline Key hkeys[1] = { { static_cast<unsigned int>(-1), Ctrl|Alt, XK_Delete, quit, {0} } };
