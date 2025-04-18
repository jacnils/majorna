/* majorna - fancy dynamic menu
 * See LICENSE file for copyright and license details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <history.hpp>
#include <macros.hpp>
#include <majorna.hpp>
#include <limhamn/primitive/primitive.hpp>
#include <options.hpp>
#include <draw.hpp>
#include <wl/wayland.hpp>
#include <wl/init.hpp>
#include <fifo.hpp>
#include <argv.hpp>
#include <stream.hpp>
#include <img.hpp>
#include <schemes.hpp>
#include <x11/init.hpp>
#include <x11/event.hpp>
#include <x11/client.hpp>
#include <match.hpp>
#include <cctype>
#include <filesystem>

int is_selected(size_t index) {
    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] == index) {
            return 1;
        }
    }

    return 0;
}

void appenditem(struct item *item, struct item **list, struct item **last) {
    if (*last)
        (*last)->right = item;
    else
        *list = item;

    item->left = *last;
    item->right = NULL;
    *last = item;
}

void recalculatenumbers() {
    unsigned int numer = 0, denom = 0, selected = 0;
    struct item *item;

    if (matchend) {
        numer++;

        for (item = matchend; item && item->left; item = item->left)
            numer++;
    }

    for (item = items; item && item->text; item++) {
        denom++;
    }

    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] == -1) {
            break;
        }

        selected++;
    }

    if (selected) {
        snprintf(strings.numbers, NUMBERSBUFSIZE, "%d/%d/%d", numer, denom, selected);
    } else {
        snprintf(strings.numbers, NUMBERSBUFSIZE, "%d/%d", numer, denom);
    }
}

void calcoffsets() {
    int i, offset;
    int numberw = 0;
    int modew = 0;
    int larroww = 0;
    int rarroww = 0;
    int capsw = 0;

    if (!hidematchcount) numberw = pango_numbers ? TEXTWM(strings.numbers) : TEXTW(strings.numbers);
    if (!hidemode) modew = pango_mode ? TEXTWM(strings.modetext) : TEXTW(strings.modetext);
    if (!hidelarrow) larroww = pango_leftarrow ? TEXTWM(leftarrow) : TEXTW(leftarrow);
    if (!hiderarrow) rarroww = pango_rightarrow ? TEXTWM(rightarrow) : TEXTW(rightarrow);
    if (!hidecaps) capsw = pango_caps ? TEXTWM(strings.capstext) : TEXTW(strings.capstext);

    if (!strcmp(strings.capstext, "")) {
        capsw = 0;
    }

    if (lines > 0) {
        offset = lines * columns * ctx.bh;
        ctx.maxlen = ctx.mw - (ctx.promptw + modew + numberw + capsw + menumarginh);
    } else { /* no lines, therefore the size of items must be decreased to fit the menu elements */
        offset = ctx.mw - (ctx.promptw + ctx.inputw + larroww + rarroww + modew + numberw + capsw + menumarginh);
        ctx.maxlen = selecteditem ? ctx.inputw : ctx.mw - (ctx.promptw + modew + numberw + capsw + (selecteditem ? larroww : 0) + (selecteditem ? rarroww : 0));
    }

    for (i = 0, nextitem = currentitem; nextitem; nextitem = nextitem->right) { // next page
        nextitem->nsgrtext = get_text_n_sgr(nextitem);

        if ((i += (lines > 0) ? ctx.bh : MIN(TEXTWM(nextitem->nsgrtext) + (powerlineitems ? !lines ? 3 * ctx.plw : 0 : 0), offset)) > offset)
            break;
    }

    for (i = 0, previousitem = currentitem; previousitem && previousitem->left; previousitem = previousitem->left) { // previous page
        previousitem->nsgrtext = get_text_n_sgr(previousitem);

        if ((i += (lines > 0) ? ctx.bh : MIN(TEXTWM(previousitem->left->nsgrtext) + (powerlineitems ? !lines ? 3 * ctx.plw : 0 : 0), offset)) > offset)
            break;
    }
}

int max_textw() {
    int len = 0;

    for (struct item *item = items; item && item->text; item++)
        len = MAX(TEXTW(item->text), len);

    return len;
}

void cleanup() {
    size_t i;

#if IMAGE
    cleanupimage();
#endif

    for (i = 0; i < hplength; ++i)
        free(hpitems[i]);

#if X11
    if (!protocol) {
        cleanup_x11(dpy);
    }
#endif

#if FIFO
    std::filesystem::remove(fifofile);
#endif

    free(sel_index);
}

char * cistrstr(const char *h, const char *n) {
    size_t i;

    if (!n[0])
        return (char *)h;

    for (; *h; ++h) {
        for (i = 0; n[i] && tolower((unsigned char)n[i]) ==
                tolower((unsigned char)h[i]); ++i);

        if (n[i] == '\0')
            return (char *)h;
    }

    return NULL;
}

void insert(const char *str, ssize_t n) {
    if (strlen(strings.text) + n > sizeof strings.text - 1)
        return;

    static char l[BUFSIZ] = "";

    if (requirematch) {
        memcpy(l, strings.text, BUFSIZ);
    }

    // move existing text out of the way, insert new text, and update cursor
    memmove(
            &strings.text[ctx.cursor + n],
            &strings.text[ctx.cursor],
            sizeof strings.text - ctx.cursor - MAX(n, 0)
    );

    if (n > 0 && str && n) {
        memcpy(&strings.text[ctx.cursor], str, n);
    }

    ctx.cursor += n;
    match();

    if (!matches && requirematch) {
        memcpy(strings.text, l, BUFSIZ);
        ctx.cursor -= n;
        match();
    }

    // output on insertion
    if (incremental) {
        puts(strings.text);
        fflush(stdout);
    }
}

size_t nextrune(int inc) {
    ssize_t rune;

    // return location of next utf8 rune in the given direction (+1 or -1)
    for (rune = ctx.cursor + inc; rune + inc >= 0 && (strings.text[rune] & 0xc0) == 0x80; rune += inc)
        ;

    return rune;
}

void resizeclient() {
#if WAYLAND
    if (protocol) {
        resizeclient_wl(&state);
    } else {
#if X11
        resizeclient_x11();
#endif
    }
#elif X11
    resizeclient_x11();
#endif
}

/* Width reserved for input when !lines is a fixed size of the menu width * inputwidth
 * This is reasonable, but in rare cases may cause input text to overlap
 * items.
 */
void get_width() {
    ctx.inputw = ctx.mw * inputwidth;
}

void get_mh() {
    int epad;

    ctx.mh = (lines + 1) * ctx.bh;
    ctx.mh += 2 * menumarginv;

    // subtract 1 line if there's nothing to draw on the top line
    if ((hideprompt && hideinput && hidemode && hidematchcount && hidecaps) && lines) {
        ctx.mh -= ctx.bh;
    }

    epad = 2 * menupaddingv;

    // the majorna window should not exceed the screen resolution height
    if (monitor.output_height && !xpos && !ypos) {
        ctx.mh = MIN(ctx.mh, monitor.output_height - epad);

        if (ctx.mh == monitor.output_height - epad) {
            lines = ((monitor.output_height - epad) / ctx.bh);
        }
    }
}

void set_mode() {
    if (!type) { // no typing allowed, require normal mode
        ctx.mode = 0;
    }

    // set default mode, must be done before the event loop or keybindings will not work
    if (mode) {
        ctx.mode = 1;
        ctx.allowkeys = 1;

        sp_strncpy(strings.modetext, instext.c_str(), sizeof(strings.modetext));
    } else {
        ctx.mode = 0;
        ctx.allowkeys = !ctx.mode;

        sp_strncpy(strings.modetext, normtext.c_str(), sizeof(strings.modetext));
    }

    if (forceinsertmode) {
        ctx.mode = 1;
        ctx.allowkeys = 1;
    }
}

void handle() {
    if (!protocol) {
#if X11
        handle_x11();

        try {
            draw.initialize_font(font);
        } catch (const std::exception &e) {
            fprintf(stderr, "majorna: %s\n", e.what());
            exit(1);
        }

        load_history();
#if IMAGE
        store_image_vars();
#endif

        if (fast && !isatty(0)) {
            if (grabkeyboard) {
                grabkeyboard_x11();
            }

            readstdin();
        } else {
            readstdin();

            if (grabkeyboard) {
                grabkeyboard_x11();
            }
        }

        set_mode();

        init_appearance(); // init colorschemes by reading arrays

        setupdisplay_x11(); // set up display and create window
#if FIFO
        init_fifo();
#endif
        eventloop_x11(); // function is a loop which checks X11 events and calls other functions accordingly
#endif
#if WAYLAND
    } else {
        load_history();
#if IMAGE
        store_image_vars();
        setimageopts();
#endif

        // Disable some X11 only features
        menupaddingv = menupaddingh = 0;
        xpos = ypos = 0;
        borderwidth = 0;
        managed = 0;

        // um???
        draw = limhamn::primitive::draw_manager();
        draw.initialize_font(font);

        readstdin();
        set_mode();
        init_appearance();

#if FIFO
        init_fifo();
#endif

        handle_wl();
#endif
    }
}

int main(int argc, char *argv[]) {
    readargs(argc, argv);

    /* pledge limits what programs can do, so here we specify what majorna should be allowed to do
     * TODO: Test this on an actual OpenBSD operating system
     */
#ifdef __OpenBSD__
    if (pledge("stdio rpath wpath cpath", NULL) == -1)
        die("pledge");
#endif

    handle();

    return 1;
}
