/* See LICENSE file for copyright and license details. */

#include <x11/mouse.hpp>
#include <x11/x11_libs.hpp>
#include <x11/init.hpp>
#include <options.hpp>
#include <majorna.hpp>
#include <macros.hpp>
#include <cstring>

void buttonpress_x11(XEvent& e) {
    item *item;
    XButtonPressedEvent *ev = &e.xbutton;

    int x = 0, y = 0, h = ctx.item_height, w, item_num = 0;
    unsigned int i, click;
    int yp = 0;

    x += menumarginh;

    int larroww = 0;
    int rarroww = 0;
    int numberw = 0;
    int modew = 0;
    int capsw = 0;

    if (!hidelarrow) larroww = pango_leftarrow ? TEXTWM(leftarrow) : TEXTW(leftarrow);
    if (!hiderarrow) rarroww = pango_rightarrow ? TEXTWM(rightarrow) : TEXTW(rightarrow);
    if (!hidematchcount) numberw = pango_numbers ? TEXTWM(strings.number_text) : TEXTW(strings.number_text);
    if (!hidemode) modew = pango_mode ? TEXTWM(strings.mode_text) : TEXTW(strings.mode_text);
    if (!hidecaps) capsw = pango_caps ? TEXTWM(strings.caps_text) : TEXTW(strings.caps_text);

    if (!strcmp(strings.caps_text, ""))
        capsw = 0;

    if ((hideprompt && hideinput && hidemode && hidematchcount && hidecaps) && lines) {
        yp = 1;
    } else if (!itemposition && lines && ev->y <= h + menumarginv && ev->y >= menumarginv) {
        yp = 1;
    } else if (itemposition && lines && ev->y >= (ctx.win_height - h) + menumarginv) {
        yp = 1;
    } else if (!lines) {
        yp = 1;
    }

    if (ev->window != win) return; // if incorrect or wrong window, return

    click = ClickWindow; // clicking anywhere, we use this and override it if we clicked on something specific

    // check click position and override the value of click
    if (yp && ev->x < x + ctx.prompt_width + powerlineprompt ? ctx.powerline_width : 0) { // prompt
        click = ClickPrompt;
    } else if (yp && (ev->x > ctx.win_width - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh) && !hidecaps && capsw) { // caps lock indicator
        click = ClickCaps;
    } else if (yp && ev->x > ctx.win_width - modew - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh) { // mode indicator
        click = ClickMode;
    } else if (yp && ev->x > ctx.win_width - modew - numberw - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh) { // match count
        click = ClickNumber;
    } else if (yp && !hideinput) { // input
        w = (lines > 0 || !matches) ? ctx.win_width - x : ctx.input_width;

        if ((lines <= 0 && ev->x >= 0 && ev->x <= x + w + ctx.prompt_width +
                    ((!previousitem || !currentitem->left) ? larroww : 0)) ||
                (lines > 0 && ev->y >= y && ev->y <= y + h)) {

            click = ClickInput;
        }
    }

#if IMAGE
    if (!hideimage && img.longest_edge != 0 && imagetype) {
        x += MAX((img.gaps * 2) + img.width, indentitems ? ctx.prompt_width : 0);
    }
#endif

    // item click
    if (lines > 0) {
        w = ctx.win_width - x;

        ev->y -= menumarginv;

        if ((hideprompt && hideinput && hidemode && hidematchcount && hidecaps) || itemposition) {
            ev->y += h;
        }

        for (item = currentitem; item != nextitem; item = item->right) {
            if (item_num++ == lines) {
                item_num = 1;
                x += w / columns;
                y = 0;
            }

            y += h;

            // ClickSelItem, called function doesn't matter
            if (ev->y >= y && ev->y <= (y + h) && ev->x >= x + (powerlineitems ? ctx.powerline_width : 0) && ev->x <= (x + w / columns) + (powerlineitems ? ctx.powerline_width : 0)) {
                click = ClickItem;
                mouseitem = item;
#if IMAGE
            } else if (ev->y >= y && ev->y <= (y + h) && ev->x >= x + (powerlineitems ? ctx.powerline_width : 0) - MAX((img.gaps * 2) + img.width, indentitems ? ctx.prompt_width : 0) && ev->x <= (x - MAX((img.gaps * 2) + img.width, indentitems ? ctx.prompt_width : 0) + w / columns) + (powerlineitems ? ctx.powerline_width : 0)) {
                click = ClickImage;
                mouseitem = item;
#endif
            }
        }
    } else if (matches) { // a single line, meaning it could be arrows too, so we check that here
        x += ctx.input_width;
        w = larroww;

        if (previousitem && currentitem->left) {
            if (ev->x >= x && ev->x <= x + w) {
                click = ClickLArrow;
            }
        }

        // right arrow
        w = rarroww;
        x = ctx.win_width - w;
        if (nextitem && ev->x >= x && ev->x <= x + w) {
            click = ClickRArrow;
        }
    }

    // go through mouse button array and run function
    for (auto& it : buttons) {
        if (ctx.ignore_conf_mouse) break;
        if ((click == it.click || it.click == ClickNone) && it.func && it.button == ev->button)
            it.func(it.arg);
    }
    for (auto& it : wl_buttons) {
        if (ctx.ignore_global_mouse) break;
        if ((click == it.click || it.click == ClickNone) && it.func && it.button == ev->button)
            it.func(it.arg);
    }
}
