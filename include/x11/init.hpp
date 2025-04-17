#pragma once

#include <x11/x11_libs.hpp>

#define CLEANMASK(mask)         (mask & ~(x11.numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)

inline Atom clip, utf8, types, dock;
inline Display *dpy;
inline Window root, parentwin, win;

void setupdisplay_x11();
void set_screen_x11(Display *disp);
void handle_x11();
void cleanup_x11(Display *disp);
void prepare_window_size_x11();
Display* opendisplay_x11(char* disp);
