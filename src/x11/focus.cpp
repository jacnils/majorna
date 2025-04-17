/* See LICENSE file for copyright and license details. */

#include <macros.hpp>
#include <time.h>
#include <cstring>
#include <majorna.hpp>
#include <options.hpp>
#include <x11/x11_libs.hpp>
#include <x11/focus.hpp>
#include <x11/init.hpp>

void grabfocus_x11(void) {
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000  };
    Window focuswin;
    int i, revertwin;
    XWindowAttributes wa;

    XSync(dpy, false);
    XGetWindowAttributes(dpy, win, &wa);

    for (i = 0; i < 100; ++i) {
        XGetInputFocus(dpy, &focuswin, &revertwin);
        if (focuswin == win)
            return;

        // if it's a client, we can't just steal all the input for ourselves
        if (managed) {
            if (wa.map_state == IsViewable) {
                XTextProperty prop;
                const char *windowtitle = prompt.empty() ? "majorna" : prompt.c_str();
                Xutf8TextListToTextProperty(dpy, const_cast<char**>(&windowtitle), 1, XUTF8StringStyle, &prop);
                XSetWMName(dpy, win, &prop);
                XSetTextProperty(dpy, win, &prop, XInternAtom(dpy, "_NET_WM_NAME", False));
                XFree(prop.value);
            }
        } else { // majorna is not managed, and is very greedy
            if (wa.map_state == IsViewable) // it must be viewable first, otherwise we get a BadMatch error
                XSetInputFocus(dpy, win, RevertToParent, CurrentTime);
        }

        nanosleep(&ts, NULL);
    }

    die("majorna: cannot grab focus"); // not possible to grab focus, abort immediately
}
