#include <x11/key.hpp>
#include <x11/x11_libs.hpp>
#include <x11/init.hpp>
#include <x11/xim.hpp>
#include <draw.hpp>
#include <majorna.hpp>
#include <cctype>

void updatenumlockmask() {
    x11.numlockmask = 0;

    XModifierKeymap* modmap = XGetModifierMapping(dpy);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j]
                    == XKeysymToKeycode(dpy, XK_Num_Lock))
                x11.numlockmask = (1 << i);

    XFreeModifiermap(modmap);
}

// called on keypress
void keypress_x11(XEvent& e) {
    updatenumlockmask();

    KeySym keysym = NoSymbol;

    char buf[64];
    Status status;

    XKeyEvent *ev = &e.xkey;

    KeySym ksym = NoSymbol;
    const int len = XmbLookupString(xic, ev, buf, sizeof buf, &ksym, &status);

    KeySym keysym_case = NoSymbol;
    XConvertCase(ksym, &keysym, &keysym_case);

    // this makes sure we always have a way to exit if we unbind our quit key
    if (keysym == hkeys[0].keysym && CLEANMASK(hkeys[0].mod) == CLEANMASK(ev->state) && hkeys[0].func) hkeys[0].func(hkeys[0].arg);

    for (auto& it : keys) {
        if (ctx.ignore_global_keys) break;

        if (keysym == it.keysym && CLEANMASK(it.mod) == CLEANMASK(ev->state) && it.func) {
            if ((it.mode && ctx.mode) || it.mode == -1) {
                it.func(it.arg);
                return;
            } else if (!it.mode && !ctx.mode) {
                it.func(it.arg);
            }
        }
    }

    for (auto& it : ckeys) {
        if (ctx.ignore_conf_keys) break;

        if (keysym == it.keysym && CLEANMASK(it.mod) == CLEANMASK(ev->state) && it.func) {
            if ((it.mode && ctx.mode) || it.mode == -1) {
                it.func(it.arg);
                return;
            } else if (!it.mode && !ctx.mode) {
                it.func(it.arg);
            }
        }
    }

    if (!iscntrl(*buf) && type && ctx.mode ) {
        if (ctx.allow_input) {
            insert(buf, len);
        } else {
            ctx.allow_input = !ctx.allow_input;
        }

        drawmenu();
    }
}

void grabkeyboard_x11() {
    constexpr timespec ts = { .tv_sec = 0, .tv_nsec = 1000000  };
    if (x11.embed || managed) {
        return;
    }

    for (int i = 0; i < 1000; i++) {
        if (XGrabKeyboard(dpy, DefaultRootWindow(dpy), True, GrabModeAsync,
                    GrabModeAsync, CurrentTime) == GrabSuccess) {
            getcapsstate();
            return;
        }
        nanosleep(&ts, nullptr);
    }

    throw std::runtime_error("cannot grab keyboard");
}

void getcapsstate() {
    unsigned int cs = 0;

    XkbGetIndicatorState(dpy, XkbUseCoreKbd, &cs);
    ctx.caps_state = (cs & 0x01) == 1;

    strncpy(strings.caps_text, ctx.caps_state ? capslockontext.c_str() : capslockofftext.c_str(), 15);
}
