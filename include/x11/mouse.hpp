#pragma once

#include <x11/x11_libs.hpp>
#include <arg.hpp>
#include <functional>

struct Mouse {
    unsigned int click{};
    unsigned int button{};
    std::function<void(Arg&)> func{};
    Arg arg{};
};

inline std::vector<Mouse> cbuttons;
void buttonpress_x11(XEvent& e);
