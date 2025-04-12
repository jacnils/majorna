/* See LICENSE file for copyright and license details. */

#include <iostream>
#include <x11/xim.hpp>
#include <x11/x11_libs.hpp>
#include <x11/init.hpp>

void open_xim(void) {
    XIM xim;

    if ((xim = XOpenIM(dpy, NULL, NULL, NULL)) == NULL) {
        XSetLocaleModifiers("@im=local");
        if ((xim = XOpenIM(dpy, NULL, NULL, NULL)) == NULL) {
            XSetLocaleModifiers("@im=");
            if ((xim = XOpenIM(dpy, NULL, NULL, NULL)) == NULL)
                std::cerr << "Warning: XOpenIM() failed, input devices may be affected.\n";
                return;
        }
    }

    xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
            XNClientWindow, win, XNFocusWindow, win, NULL);
}
