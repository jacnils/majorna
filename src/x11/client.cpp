/* See LICENSE file for copyright and license details. */

#include <x11/client.hpp>
#include <options.hpp>
#include <majorna.hpp>
#include <macros.hpp>
#include <x11/x11_libs.hpp>
#include <x11/init.hpp>
#include <img.hpp>
#include <iostream>

void hexconv(const char *hex, unsigned short *r, unsigned short *g, unsigned short *b) {
    unsigned int col;

    sscanf(hex, "#%06X", &col);

    *r = (col >> 16) & 0xFF;
    *g = (col >> 8) & 0xFF;
    *b = col & 0xFF;
}

void create_window_x11(int x, int y, int w, int h) {
    XSetWindowAttributes swa;

    swa.override_redirect = managed ? False : True;
    swa.background_pixel = 0;
    swa.colormap = x11.cmap;
    swa.event_mask =
        ExposureMask | // mapping the drawing
        KeyPressMask | // keypresses
        VisibilityChangeMask | // whether or not client is visible
        ButtonPressMask |  // see buttonpress in mouse.c for usage
        PointerMotionMask; // we need pointer for selecting entries using the mouse

    // create client
    win = XCreateWindow(dpy, root, x, y, w, h, borderwidth,
            x11.depth, InputOutput, x11.visual,
            CWOverrideRedirect|CWBackPixel|CWBorderPixel|CWColormap|CWEventMask, &swa);
}

void set_window_x11() {
    XColor col;
    XClassHint ch = { strdup(window_class.c_str()), strdup(window_class.c_str()) };

    unsigned short r{};
    unsigned short g{};
    unsigned short b{};

    hexconv(col_border.c_str(), &r, &g, &b);

    col.red = r << 8;
    col.green = g << 8;
    col.blue = b << 8;

    if (!XAllocColor(dpy, x11.cmap, &col)) {
        die("majorna: failed to allocate xcolor");
    }

    // set border and class
    XSetWindowBorder(dpy, win, col.pixel);
    XSetClassHint(dpy, win, &ch);

    return;
}

void set_prop_x11(void) {
    // set properties indicating what majorna handles
    clip = XInternAtom(dpy, "CLIPBOARD",   False);
    utf8 = XInternAtom(dpy, "UTF8_STRING", False);
    types = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);

    // set dock property
    if (dockproperty) {
        dock = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
        XChangeProperty(dpy, win, types, XA_ATOM, 32, PropModeReplace, (unsigned char *) &dock, 1); // set dock property
    }
}

void resizeclient_x11(void) {
    int x, y;
    struct item *item;
    int ic = 0; // item count

    // walk through all items
    for (item = items; item && item->raw_text; item++)
        ic++;

    lines = MAX(MIN(ic, MAX(lines, 0)), minlines);
#if IMAGE
    img.set_lines = lines;

    // resize client to image height
    if (image) resizetoimageheight(img.height);
#endif

    get_mh();

    if (hideprompt && hideinput && hidemode && hidematchcount && hidecaps) {
        ctx.win_height -= ctx.item_height;
    }

    if (menuposition == 2) { // centered
        ctx.win_width = MIN(MAX(max_textw() + ctx.prompt_width, centerwidth), monitor.output_width);
        x = monitor.output_xpos + ((monitor.output_width  - ctx.win_width) / 2 + xpos);
        y = monitor.output_ypos + ((monitor.output_height - ctx.win_height) / 2 - ypos);
    } else { // top or bottom
        x = monitor.output_xpos + xpos;
        y = monitor.output_ypos + menuposition ? (-ypos) : (monitor.output_height - ctx.win_height - ypos);
        ctx.win_width = (menuwidth > 0 ? menuwidth : monitor.output_width);
    }

    if (win) {
        XMoveResizeWindow(dpy, win, x + ctx.hpadding, y + ctx.vpadding, ctx.win_width - 2 * ctx.hpadding - borderwidth * 2, ctx.win_height);
        draw.resize({
            .w = ctx.win_width - 2 * ctx.hpadding - borderwidth * 2,
            .h = ctx.win_height,
        });
    }
}

void xinitvisual(void) {
    XVisualInfo *infos;
    XRenderPictFormat *fmt;
    int nitems;
    int i;

    // visual properties
    XVisualInfo tpl;

    tpl = {
        .screen = x11.screen,
        .depth = 32,
        .c_class = TrueColor,
    };

    long masks = VisualScreenMask | VisualDepthMask | VisualClassMask;

    infos = XGetVisualInfo(dpy, masks, &tpl, &nitems);
    x11.visual = NULL;

    // create colormap
    for(i = 0; i < nitems; i ++) {
        fmt = XRenderFindVisualFormat(dpy, infos[i].visual);
        if (fmt->type == PictTypeDirect && fmt->direct.alphaMask) {
            x11.visual = infos[i].visual;
            x11.depth = infos[i].depth;
            x11.cmap = XCreateColormap(dpy, root, x11.visual, AllocNone);
            x11.use_argb = 1;
            break;
        }
    }

    XFree(infos);

    // no alpha, reset to default
    if (!x11.visual || !alpha) {
        x11.visual = DefaultVisual(dpy, x11.screen);
        x11.depth = DefaultDepth(dpy, x11.screen);
        x11.cmap = DefaultColormap(dpy, x11.screen);
    }
}
