#pragma once

#include <x11/x11_libs.hpp>
#include <arg.hpp>
#include <mouse.hpp>
#include <functional>


#if X11
enum class XButtonType {
    LeftClick = Button1,
    MiddleClick = Button2,
    RightClick = Button3,
    ScrollUp = Button4,
    ScrollDown = Button5,
};
#endif

struct Mouse {
    ClickType click{};
    XButtonType button{};
    std::function<void(Arg&)> func{};
    Arg arg{};
};

void buttonpress_x11(XEvent& e);
