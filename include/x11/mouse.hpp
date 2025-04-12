#pragma once

#include <x11/x11_libs.hpp>
#include <arg.hpp>

typedef struct {
    unsigned int click;
    unsigned int button;
    void (*func)(Arg *arg);
    Arg arg;
} Mouse;

inline Mouse cbuttons[256];
void buttonpress_x11(XEvent *e);
