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

    for (unsigned int i = 0; i < LENGTH(keys); i++) {
        if (sp.ignoreglobalkeys) break;
        if (keysym == keys[i].keysym && CLEANMASK(keys[i].mod) == CLEANMASK(ev->state) && keys[i].func) {
            if ((keys[i].mode && sp.mode) || keys[i].mode == -1) {
                keys[i].func(keys[i].arg);
                return;
            } else if (!keys[i].mode && !sp.mode) {
                keys[i].func(keys[i].arg);
            }
        }
    }

    for (unsigned int i = 0; i < LENGTH(ckeys); i++) {
        if (sp.ignoreconfkeys) break;
        if (keysym == ckeys[i].keysym && CLEANMASK(ckeys[i].mod) == CLEANMASK(ev->state) && ckeys[i].func) {
            if ((ckeys[i].mode && sp.mode) || ckeys[i].mode == -1) {
                ckeys[i].func(ckeys[i].arg);
                return;
            } else if (!ckeys[i].mode && !sp.mode) {
                ckeys[i].func(ckeys[i].arg);
            }
        }
    }

    if (!iscntrl(*buf) && type && sp.mode ) {
        if (sp.allowkeys) {
            insert(buf, len);
        } else {
            sp.allowkeys = !sp.allowkeys;
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
    sp.capslockstate = (cs & 0x01) == 1;

    strncpy(tx.capstext, sp.capslockstate ? capslockontext.c_str() : capslockofftext.c_str(), 15);
}
