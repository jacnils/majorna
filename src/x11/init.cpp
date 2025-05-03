#include <x11/init.hpp>
#include <x11/x11_libs.hpp>
#include <x11/client.hpp>
#include <img.hpp>
#include <majorna.hpp>
#include <options.hpp>
#include <macros.hpp>
#include <draw.hpp>
#include <clocale>
#include <match.hpp>
#include <x11/xim.hpp>
#include <x11/focus.hpp>
#include <thread>

void setupdisplay_x11() {
    int x, y, i;
    unsigned int du;

    Window w, dw, *dws;
    XWindowAttributes wa;

    prepare_window_size_x11();

    // resize client to image height if deemed necessary
#if IMAGE
    if (image) resizetoimageheight(img.height);
#endif

    // set prompt width based on prompt size
    ctx.prompt_width = !prompt.empty() ? pango_prompt ? TEXTWM(prompt) : TEXTW(prompt) - ctx.lrpad / 4 : 0; // prompt width

    // init xinerama screens
#if XINERAMA
    XineramaScreenInfo *info;
    Window pw;
    int a, n, area = 0;
    int j, di;

    i = 0;
    if (parentwin == root && (info = XineramaQueryScreens(dpy, &n))) {
        XGetInputFocus(dpy, &w, &di);
        if (mon >= 0 && mon < n)
            i = mon;
        else if (w != root && w != PointerRoot && w != None) {
            // find top-level window containing current input focus
            do {
                if (XQueryTree(dpy, (pw = w), &dw, &w, &dws, &du) && dws)
                    XFree(dws);
            } while (w != root && w != pw);
            // find xinerama screen with which the window intersects most
            if (XGetWindowAttributes(dpy, pw, &wa))
                for (j = 0; j < n; j++)
                    if ((a = INTERSECT(wa.x, wa.y, wa.width, wa.height, info[j])) > area) {
                        area = a;
                        i = j;
                    }
        }
        // no focused window is on screen, so use pointer location instead
        if (mon < 0 && !area && XQueryPointer(dpy, root, &dw, &dw, &x, &y, &di, &di, &du))
            for (i = 0; i < n; i++)
                if (INTERSECT(x, y, 1, 1, info[i]))
                    break;

        monitor.output_width = info[i].width;
        monitor.output_height = info[i].height;
        monitor.output_xpos = info[i].x_org;
        monitor.output_ypos = info[i].y_org;

        XFree(info);
    } else
#endif
    {
        if (!XGetWindowAttributes(dpy, parentwin, &wa))
            die("majorna: could not get embedding window attributes: 0x%lx",
                    parentwin); // die because unable to get attributes for the parent window

        monitor.output_width = wa.width;
        monitor.output_height = wa.height;
    }

    get_mh();

    if (menuposition == 2) { // centered
        ctx.win_width = MIN(MAX(max_textw() + ctx.prompt_width, centerwidth), monitor.output_width);
        x = monitor.output_xpos + ((monitor.output_width  - ctx.win_width) / 2 + xpos);
        y = monitor.output_ypos + ((monitor.output_height - ctx.win_height) / 2 - ypos);
    } else { // top or bottom
        x = monitor.output_xpos + xpos;
        y = monitor.output_ypos + menuposition ? (-ypos) : (monitor.output_height - ctx.win_height - ypos);
        ctx.win_width = (menuwidth > 0 ? menuwidth : monitor.output_width);
    }

    // create menu window and set properties for it
    create_window_x11(
            x + ctx.hpadding,
            y + ctx.vpadding - (menuposition == 1 ? 0 : menuposition == 2 ? borderwidth : borderwidth * 2),
            ctx.win_width - 2 * ctx.hpadding - borderwidth * 2,
            ctx.win_height
    );

    set_window_x11();
    set_prop_x11();

#if IMAGE
    setimageopts();
#endif

    open_xim(); // open xim

    XMapRaised(dpy, win);
    XSync(dpy, False);
    XGetWindowAttributes(dpy, win, &wa);

    if (wa.map_state == IsViewable) { // must be viewable, otherwise we get a BadMatch error
        XSetInputFocus(dpy, win, RevertToParent, CurrentTime);
    }

    // embed majorna inside parent window
    if (x11.embed) {
        XReparentWindow(dpy, win, parentwin, x, y);
        XSelectInput(dpy, parentwin, FocusChangeMask | SubstructureNotifyMask);

        if (XQueryTree(dpy, parentwin, &dw, &w, &dws, &du) && dws) {
            for (i = 0; i < du && dws[i] != win; ++i)
                XSelectInput(dpy, dws[i], FocusChangeMask);
            XFree(dws);
        }

        grabfocus_x11();
    }

    // resize window and draw
    draw.resize({
        .w = ctx.win_width - 2 * ctx.hpadding - borderwidth * 2,
        .h = ctx.win_height,
    });

    match();

    if (!listfile.empty()) {
	    auto running = std::make_shared<std::atomic<bool>>(true);
	    std::thread t([running]() {
		    while (running->load()) {
			draw_menu();
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		    }
	    });

	    t.detach();
    } else {
	    draw_menu();
    }
}

void prepare_window_size_x11() {
    ctx.hpadding = menupaddingh;
    ctx.vpadding = (menuposition == 1) ? menupaddingv : - menupaddingv;

    ctx.item_height = std::max(ctx.item_height, static_cast<int>(draw.get_font_manager().get_height() + 2 + lineheight));
    lines = MAX(lines, 0);
#if IMAGE
    img.set_lines = lines;
#endif

    ctx.lrpad = draw.get_font_manager().get_height() + textpadding;
    get_mh();
}

Display * opendisplay_x11(char *disp) {
    return XOpenDisplay(disp);
}

void set_screen_x11(Display *disp) {
    x11.screen = DefaultScreen(disp);
    root = RootWindow(disp, x11.screen);
}

void handle_x11() {
    XWindowAttributes wa;

    if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
        fputs("warning: no locale support\n", stderr); // invalid locale, so notify the user about it

    if (!XSetLocaleModifiers(""))
        fputs("warning: no locale modifiers support\n", stderr);

    if (!(dpy = opendisplay_x11(NULL)))
        die("majorna: cannot open display"); // failed to open display

    // set screen and root window
    set_screen_x11(dpy);

    // parent window is the root window (ie. window manager) because we're not embedding
    if (!x11.embed || !(parentwin = strtol(x11.embed, NULL, 0)))
        parentwin = root;

    if (!XGetWindowAttributes(dpy, parentwin, &wa)) {
        die("majorna: could not get embedding window attributes: 0x%lx", parentwin);
    }

    xinitvisual(); // init visual and create drawable after
    draw.initialize_x11(dpy, x11.screen, root, wa.width, wa.height, x11.visual, x11.depth, x11.cmap);
}

void cleanup_x11(Display *disp) {
    if (disp) {
        XUngrabKey(disp, AnyKey, AnyModifier, root);
        XSync(disp, False);
        disp = nullptr;
    }
}
