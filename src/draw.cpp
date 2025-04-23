#include <draw.hpp>
#include <majorna.hpp>
#include <limhamn/primitive/primitive.hpp>
#include <Imlib2.h>
#include <x11/init.hpp>
#include <img.hpp>
#include <match.hpp>
#include <stream.hpp>
#include <rtl.hpp>
#include <schemes.hpp>
#include <unistd.h>
#include <iomanip>
#include <algorithm>
#include <atomic>

void drawhighlights(item *item, int x, int y, int w, int p, char *itemtext) {
	char restorechar, text[sizeof strings.input_text], *highlight,  *ctext;
	int indent, highlightlen;

    if ((columns > 5 && lines > 1) || (!(strlen(itemtext) && strlen(strings.input_text))) || strstr(itemtext, "</") || is_selected(item->index)) {
        return;
    }

	strcpy(text, strings.input_text);

	for (ctext = text; ctext; ctext = nullptr) {
		highlight = fstrstr(itemtext, ctext);

		while (highlight) {
			highlightlen = highlight - itemtext;
			restorechar = *highlight;

			itemtext[highlightlen] = '\0';

			indent = TEXTW(itemtext);
			itemtext[highlightlen] = restorechar;

			restorechar = highlight[strlen(ctext)];
			highlight[strlen(ctext)] = '\0';

			if (indent - (ctx.lrpad / 2) - 1 < w) {
			    /*
                draw_text(
                    draw,
                    x + indent - (sp.lrpad / 2) - 1,
                    y,
					MIN(w - indent, TEXTW(highlight) - sp.lrpad),
                    sp.bh, 0, highlight, 0, False,
                    item == selecteditem ? col_hlselfg : col_hlnormfg,
                    item == selecteditem ? col_hlselbg : col_hlnormbg,
                    item == selecteditem ? alpha_hlselfg : alpha_hlnormfg,
                    item == selecteditem ? alpha_hlselbg : alpha_hlnormbg
                );
                */
			    draw.draw_text({
			        .x = x + indent - (ctx.lrpad / 2) - 1,
			        .y = y,
			        .w = static_cast<int>(MIN(w - indent, TEXTW(highlight) - ctx.lrpad)),
			        .h = ctx.item_height,
			    }, 0, highlight, false, {
			        .foreground = item == selecteditem ? col_hlselfg : col_hlnormfg,
			        .background = item == selecteditem ? col_hlselbg : col_hlnormbg,
			        .foreground_alpha = item == selecteditem ? alpha_hlselfg : alpha_hlnormfg,
			        .background_alpha = item == selecteditem ? alpha_hlselbg : alpha_hlnormbg,
			    });
            }

			highlight[strlen(ctext)] = restorechar;

			if (strlen(highlight) - strlen(ctext) < strlen(ctext))
				break;

			highlight = fstrstr(highlight + strlen(ctext), ctext);
		}
	}
}

char* get_text_n_sgr(item *item) {
    char buffer[MAXITEMLENGTH];
    int character, escape;

    char *sgrtext = static_cast<char*>(malloc(sizeof(buffer)));
    sgrtext[0] = '\0';

    for (character = 0, escape = 0; item->raw_text[escape]; escape++) {
        if (item->raw_text[escape] == '' && item->raw_text[escape + 1] == '[') {
            size_t colindex = strspn(item->raw_text + escape + 2, "0123456789;");
            if (item->raw_text[escape + colindex + 2] == 'm' && sgr) { // last character in sequence is always 'm'
                buffer[character] = '\0';

                strcat(sgrtext, buffer);

                escape += colindex + 2;
                character = 0;

                continue;
            }
        }

        buffer[character++] = item->raw_text[escape];
    }

    buffer[character] = '\0';
    strcat(sgrtext, buffer);

    return sgrtext;
}

int drawitemtext(item *item, int x, int y, int w) {
    char buffer[MAXITEMLENGTH]; // buffer containing item text
    int character, escape;
    int leftpadding = ctx.lrpad / 2; // padding
    int fg = 7; // foreground
    int bg = 0; // background
    int bgfg = 0; // both
    int ignore = 0; // ignore colors
    int priitem = 0;
    char *bgcol;
    char *fgcol;
    char *obgcol;
    char *ofgcol;
    int bga;
    int fga;
    int obga;
    int ofga;

    int ox;
    int oy;
    int ow;

    int oleftpadding;

    // memcpy the correct scheme
    if (item == selecteditem) {
        bgcol = strdup(col_itemselbg.c_str());
        fgcol = strdup(col_itemselfg.c_str());
        bga = alpha_itemselbg;
        fga = alpha_itemselfg;

        if (item->high_priority) {
            priitem = 1;
            bgcol = strdup(col_itemselpribg.c_str());
            fgcol = strdup(col_itemselprifg.c_str());

            fga = alpha_itemselprifg;
            bga = alpha_itemselpribg;
        }
    } else {
        if (itemn) {
            bgcol = strdup(col_itemnormbg2.c_str());
            fgcol = strdup(col_itemnormfg2.c_str());
            fga = alpha_itemnormfg2;
            bga = alpha_itemnormbg2;
        } else {
            bgcol = strdup(col_itemnormbg.c_str());
            fgcol = strdup(col_itemnormfg.c_str());
            fga = alpha_itemnormfg;
            bga = alpha_itemnormbg;
        }

        if (item->high_priority) {
            priitem = 1;
            bgcol = strdup(col_itemnormpribg.c_str());
            fgcol = strdup(col_itemnormprifg.c_str());
            fga = alpha_itemnormprifg;
            bga = alpha_itemnormpribg;
        }
    }

    if (is_selected(item->index)) {
        bgcol = strdup(col_itemmarkedbg.c_str());
        fgcol = strdup(col_itemmarkedfg.c_str());
        fga = alpha_itemmarkedfg;
        bga = alpha_itemmarkedbg;
    }

    // apply extra padding
    if ((!priitem) && lines) {
        leftpadding += selitempadding;
    } else if (priitem && lines) {
        leftpadding += priitempadding;
    } else if (lines) {
        leftpadding += normitempadding;
    }

    // don't color
    if (!coloritems) {
        bgcol = itemn ? strdup(col_itemnormbg2.c_str()) : strdup(col_itemnormbg.c_str());
        fgcol = itemn ? strdup(col_itemnormfg2.c_str()) : strdup(col_itemnormfg.c_str());
        bga = itemn ? alpha_itemnormbg2 : alpha_itemnormbg;
        fga = itemn ? alpha_itemnormfg2 : alpha_itemnormfg;
    }

    ofga = fga;
    obga = bga;
    ofgcol = fgcol;
    obgcol = bgcol;
    oleftpadding = leftpadding;

    if (!hidepowerline && powerlineitems) {
        if (itempwlstyle == 2) {
            //draw_circle(draw, x - sp.plw, y, sp.plw, sp.bh, 0, col_menu, bgcol, alpha_menu, bga);
            draw.draw_circle({
                .x = x - ctx.powerline_width,
                .y = y,
                .w = ctx.powerline_width,
                .h = ctx.item_height,
            }, 0, {
                .prev = col_menu,
                .next = bgcol,
                .prev_alpha = alpha_menu,
                .next_alpha = bga,
            });
        } else {
            //draw_arrow(draw, x - sp.plw, y, sp.plw, sp.bh, 0, itempwlstyle, col_menu, bgcol, alpha_menu, bga);
            draw.draw_arrow({
                .x = x - ctx.powerline_width,
                .y = y,
                .w = ctx.powerline_width,
                .h = ctx.item_height,
            }, 0, itempwlstyle, {
                .prev = col_menu,
                .next = bgcol,
                .prev_alpha = alpha_menu,
                .next_alpha = bga,
            });
        }
    }

#if IMAGE
    if (!hideimage && !imagetype) {
        draw.draw_rect({
            .x = x,
            .y = y,
            .w = w,
            .h = ctx.item_height,
        }, {
            .foreground = fgcol,
            .background = bgcol,
            .foreground_alpha = fga,
            .background_alpha = bga,
            .invert = 1, /* this line */
            .filled = 1, /* and this line */
        });
        int nx = draw_icon(item, x, y + ctx.lrpad / 4, ctx.item_height - ctx.lrpad / 2, ctx.item_height - ctx.lrpad / 2);

        if (nx != x) {
            x = nx;
            w -= ctx.item_height - ctx.lrpad / 2;
        }
    }
#endif

    ox = x;
    oy = y;
    ow = w;

    // parse item text
    for (character = 0, escape = 0; item->raw_text[escape]; escape++) {
        if (item->raw_text[escape] == '' && item->raw_text[escape + 1] == '[') {
            size_t colindex = strspn(item->raw_text + escape + 2, "0123456789;");
            if (item->raw_text[escape + colindex + 2] == 'm' && sgr) { // last character in sequence is always 'm'
                buffer[character] = '\0';

                apply_fribidi(buffer);
                //draw_text(draw, x, y, w, sp.bh, leftpadding, buffer, 0, pango_item ? True : False, fgcol, bgcol, fga, bga);
                draw.draw_text({
                    .x = x,
                    .y = y,
                    .w = static_cast<int>(w),
                    .h = ctx.item_height,
                }, leftpadding, buffer, pango_item ? true : false, {
                    .foreground = fgcol,
                    .background = bgcol,
                    .foreground_alpha = fga,
                    .background_alpha = bga,
                    .filled = true,
                });

                x += MIN(w, TEXTW(buffer) - ctx.lrpad + leftpadding);
                w -= MIN(w, TEXTW(buffer) - ctx.lrpad + leftpadding);

                leftpadding = 0;

                char *c_character = item->raw_text + escape + 1; // current character

                // parse hex colors, m is always the last character
                while (*c_character != 'm') {
                    unsigned nextchar = strtoul(c_character + 1, &c_character, 10);
                    if (ignore)
                        continue;
                    if (bgfg) {
                        if (bgfg < 4 && nextchar == 5) {
                            bgfg <<= 1;
                            continue;
                        }
                        if (bgfg == 4) {
                            fgcol = strdup(textcolors[fg = nextchar].c_str());
                        } else if (bgfg == 6) {
                            bgcol = strdup(textcolors[bg = nextchar].c_str());
                        }

                        ignore = 1;
                        continue;
                    }

                    if (nextchar == 1) {
                        fg |= 8;
                        fgcol = strdup(textcolors[fg].c_str());
                    } else if (nextchar == 22) {
                        fg &= ~8;
                        fgcol = strdup(textcolors[fg].c_str());
                    } else if (nextchar == 38) {
                        int r, g, b, c;

                        c = strtoul(c_character + 1, nullptr, 10);

                        if (c == 5) {
                            bgfg = 2;
                            continue;
                        }

                        strtoul(c_character + 1, &c_character, 10);

                        r = strtoul(c_character + 1, &c_character, 10);
                        g = strtoul(c_character + 1, &c_character, 10);
                        b = strtoul(c_character + 1, &c_character, 10);

                        fgcol = static_cast<char*>(malloc(8 * sizeof(char)));
                        snprintf(fgcol, 8, "#%02X%02X%02X", r, g, b);
                    } else if (nextchar == 39) {
                        fga = ofga;
                        fgcol = ofgcol;
                    } else if (nextchar >= 30 && nextchar <= 37) {
                        fg = nextchar % 10 | (fg & 8);
                        fgcol = strdup(textcolors[fg].c_str());
                    } else if (nextchar >= 40 && nextchar <= 47) {
                        bg = nextchar % 10;
                        bgcol = strdup(textcolors[bg].c_str());
                    } else if (nextchar >= 90 && nextchar <= 97) {
                        fg = (nextchar - 60) % 10 | (fg & 8);
                        fgcol = strdup(textcolors[fg].c_str());
                    } else if (nextchar >= 100 && nextchar <= 107) {
                        bg = (nextchar - 60) % 10;
                        bgcol = strdup(textcolors[bg].c_str());
                    } else if (nextchar == 48) {
                        int r, g, b, c;

                        c = strtoul(c_character + 1, NULL, 10);

                        if (c == 5) {
                            bgfg = 3;
                            continue;
                        }

                        strtoul(c_character + 1, &c_character, 10);

                        r = strtoul(c_character + 1, &c_character, 10);
                        g = strtoul(c_character + 1, &c_character, 10);
                        b = strtoul(c_character + 1, &c_character, 10);

                        bgcol = static_cast<char*>(malloc(8 * sizeof(char)));
                        snprintf(bgcol, 8, "#%02X%02X%02X", r, g, b);
                    } else if (nextchar == 49) {
                        bga = obga;
                        bgcol = obgcol;
                    } else if (nextchar == 0) {
                        fgcol = ofgcol;
                        bgcol = obgcol;
                        fga = ofga;
                        bga = obga;
                    }
                }

                escape += colindex + 2;
                character = 0;

                continue;
            }
        }

        buffer[character++] = item->raw_text[escape];
    }

    buffer[character] = '\0';

    // now draw any non-colored text
    apply_fribidi(buffer);
    //int ret = draw_text(draw, x, y, w, sp.bh, leftpadding, buffer, 0, pango_item ? True : False, fgcol, bgcol, fga, bga);
    int ret = draw.draw_text({
        .x = x,
        .y = y,
        .w = static_cast<int>(w),
        .h = ctx.item_height,
    }, leftpadding, buffer, pango_item ? true : false, {
        .foreground = fgcol,
        .background = bgcol,
        .foreground_alpha = fga,
        .background_alpha = bga,
        .filled = true,
    });

    if (!hidehighlight)
        drawhighlights(item, ox, oy, ow, oleftpadding, item->text_without_sequences);

    if (!hidepowerline && powerlineitems) {
        if (itempwlstyle == 2) {
            //draw_circle(draw, ret, y, sp.plw, sp.bh, 1, col_menu, obgcol, alpha_menu, obga);
            draw.draw_circle({
                .x = ret,
                .y = y,
                .w = ctx.powerline_width,
                .h = ctx.item_height,
            }, 1, {
                .prev = col_menu,
                .next = obgcol,
                .prev_alpha = alpha_menu,
                .next_alpha = obga,
            });
        } else {
            //draw_arrow(draw, ret, y, sp.plw, sp.bh, 1, itempwlstyle, col_menu, obgcol, alpha_menu, obga);
            draw.draw_arrow({
                .x = ret,
                .y = y,
                .w = ctx.powerline_width,
                .h = ctx.item_height,
            }, 1, itempwlstyle, {
                .prev = col_menu,
                .next = obgcol,
                .prev_alpha = alpha_menu,
                .next_alpha = obga,
            });
        }
    }

    return ret;
}


int drawitem(int x, int y, int w) {
    int numberw = 0;
    int modew = 0;
    int larroww = 0;
    int rarroww = 0;
    int capsw = 0;

    // add width
    if (!hidelarrow) larroww = pango_leftarrow ? TEXTWM(leftarrow) : TEXTW(leftarrow);
    if (!hiderarrow) rarroww = pango_rightarrow ? TEXTWM(rightarrow) : TEXTW(rightarrow);
    if (!hidemode) modew = pango_mode ? TEXTWM(strings.mode_text) : TEXTW(strings.mode_text);
    if (!hiderarrow) rarroww = pango_rightarrow ? TEXTWM(rightarrow) : TEXTW(rightarrow);
    if (!hidematchcount) numberw = pango_numbers ? TEXTWM(strings.number_text) : TEXTW(strings.number_text);
    if (!hidecaps) capsw = pango_caps ? TEXTWM(strings.caps_text) : TEXTW(strings.caps_text);

    if (!strcmp(strings.caps_text, ""))
        capsw = 0;

#if IMAGE
    int ox = 0; // original x position
#endif

    // draw items and image
    if (lines > 0) {
        int i = 0;
        int rx = 0;

        // draw image first
#if IMAGE
        if (!hideimage && img.longest_edge != 0 && imagetype) {
            rx = ox;
            rx += MAX((img.gaps * 2) + img.width + menumarginh, indentitems ? x : 0);
        } else
#endif
            if (!indentitems) {
                rx = 0;
            } else {
                rx = x;
            }

        int itemoverride = 1;
        itemn = 0;

        for (item* item = currentitem; item != nextitem; item = item->right, i++) {
            item->text_without_sequences = get_text_n_sgr(item);

            x = drawitemtext(
                    item,
                    rx + menumarginh + ((i / lines) *  ((ctx.win_width - rx) / columns)) + (powerlineitems ? ctx.powerline_width : 0),
                    y + (((i % lines) + 1) * ctx.item_height),
                    (ctx.win_width - rx) / columns - (powerlineitems ? 2 * ctx.powerline_width : 0) - (2 * menumarginh)
            );

            if (item == selecteditem && itemoverride) {
                ctx.item_number = i;
                itemoverride = 0;
            }

            itemn = !itemn;
        }

        // horizontal list
    } else if (matches) {
        x += ctx.input_width;

        if (!hidelarrow) {
            w = larroww;
            x = drawlarrow(x, y, w);
        }

        ctx.item_number = 0;
        int itemoverride = 1;

        for (item* item = currentitem; item != nextitem; item = item->right) { // draw items
            item->text_without_sequences = get_text_n_sgr(item);

            x = drawitemtext(item, x + (powerlineitems ? 2 * ctx.powerline_width : 0), y, MIN(pango_item ? TEXTWM(item->text_without_sequences) : TEXTW(item->text_without_sequences),
                        ctx.win_width - x -
                        rarroww -
                        numberw -
                        modew -
                        capsw -
                        menumarginh -
                        2 * ctx.hpadding -
                        2 * borderwidth -
                        (powerlineitems ? 2 * ctx.powerline_width : 0)
                        ));

            if (itemoverride) {
                ctx.item_number++;
            }

            if (item == selecteditem) {
                itemoverride = 0;
            }
        }

        if (!hiderarrow) {
            w = rarroww + numberw + modew + capsw + menumarginh + 2 * ctx.hpadding + 2 * borderwidth;
            x = drawrarrow(ctx.win_width - w, y, w);
        }
    }

    return x;
}

int drawprompt(int x, int y, int w) {
    if (!prompt.empty() && !hideprompt) {
        //x = draw_text(draw, x, y, w, sp.bh, sp.lrpad / 2, prompt, 0, pango_prompt ? True : False, col_promptfg, col_promptbg, alpha_promptfg, alpha_promptbg);
        x = draw.draw_text({
            .x = x,
            .y = y,
            .w = w,
            .h = ctx.item_height,
        }, ctx.lrpad / 2, prompt, pango_prompt, {
            .foreground = col_promptfg,
            .background = col_promptbg,
            .foreground_alpha = alpha_promptfg,
            .background_alpha = alpha_promptbg,
        });

        if (!hidepowerline && powerlineprompt) {
            if (promptpwlstyle == 2) {
                //draw_circle(draw, x, y, sp.plw, sp.bh, 1, col_menu, col_promptbg, alpha_menu, alpha_promptbg);
                draw.draw_circle({
                    .x = x,
                    .y = y,
                    .w = ctx.powerline_width,
                    .h = ctx.item_height,
                }, 1, {
                    .prev = col_menu,
                    .next = col_promptbg,
                    .prev_alpha = alpha_menu,
                    .next_alpha = alpha_promptbg,
                });
            } else {
                //draw_arrow(draw, x, y, sp.plw, sp.bh, 1, promptpwlstyle, col_menu, col_promptbg, alpha_menu, alpha_promptbg);
                draw.draw_arrow({
                    .x = x,
                    .y = y,
                    .w = ctx.powerline_width,
                    .h = ctx.item_height,
                }, 1, promptpwlstyle, {
                    .prev = col_menu,
                    .next = col_promptbg,
                    .prev_alpha = alpha_menu,
                    .next_alpha = alpha_promptbg,
                });
            }

            x += ctx.powerline_width;
        }
    }

    return x;
}

int drawinput(int x, int y, int w) {
    std::string censort;
    unsigned int curpos = 0;
    int fh = caretheight;
    int fw = MAX(2, caretwidth);
    int fp = caretpadding;

    if (fh > ctx.item_height) {
        fh = ctx.item_height;
    } else if (!fh) {
        fh = ctx.item_height - ctx.lrpad / 4;
    }

    if (passwd && !hideinput) {
        for (int i = 0; i < strlen(strings.input_text); i++) {
            censort += password;
        }

        apply_fribidi(censort);
        //draw_text(draw, x, y, w, sp.bh, sp.lrpad / 2, isrtl ? fribidi_text : censort, 0, pango_password ? True : False, col_inputfg, col_inputbg, alpha_inputfg, alpha_inputbg);
        draw.draw_text({
            .x = x,
            .y = y,
            .w = w,
            .h = ctx.item_height,
        }, ctx.lrpad / 2, censort, pango_password, {
            .foreground = col_inputfg,
            .background = col_inputbg,
            .foreground_alpha = alpha_inputfg,
            .background_alpha = alpha_inputbg,
        });

        curpos = TEXTW(censort) - TEXTW(&strings.input_text[ctx.cursor]);
    } else if (!passwd) {
        if (strlen(strings.input_text) && !hideinput) {
            char ptext[BUFSIZ];
            char *p;

            memcpy(ptext, strings.input_text, BUFSIZ);

            p = ptext;

            while (TEXTW(p) > ctx.max_length) {
                p++;
            }

            memmove(ptext, p, strlen(p) + 1);

            apply_fribidi(ptext);
            //draw_text(draw, x, y, w, sp.bh, sp.lrpad / 2, isrtl ? fribidi_text : ptext, 0, pango_input ? True : False, col_inputfg, col_inputbg, alpha_inputfg, alpha_inputbg);
            draw.draw_text({
                .x = x,
                .y = y,
                .w = w,
                .h = ctx.item_height,
            }, ctx.lrpad / 2, ptext, pango_input, {
                .foreground = col_inputfg,
                .background = col_inputbg,
                .foreground_alpha = alpha_inputfg,
                .background_alpha = alpha_inputbg,
            });

            curpos = TEXTW(ptext) - TEXTW(&ptext[ctx.cursor]);
        } else if (!pretext.empty()) {
            if (hidepretext) {
                pretext = "";
            }

            apply_fribidi(pretext);
            //draw_text(draw, x + fw, y, w, sp.bh, sp.lrpad / 2, isrtl ? fribidi_text : pretext, 0, pango_pretext ? True : False, col_pretextfg, col_pretextbg, alpha_pretextfg, alpha_pretextbg);
            draw.draw_text({
                .x = x + fw,
                .y = y,
                .w = w,
                .h = ctx.item_height,
            }, ctx.lrpad / 2, pretext, pango_pretext, {
                .foreground = col_pretextfg,
                .background = col_pretextbg,
                .foreground_alpha = alpha_pretextfg,
                .background_alpha = alpha_pretextbg,
            });
        }
    }

    if ((curpos += ctx.lrpad / 2 - 1) < w && !hidecaret && !hideinput) {
        curpos += fp;
        //draw_rect(draw, x + curpos, 2 + (sp.bh - fh) / 2 + y, fw, fh - 4, 1, 0, col_caretfg, col_caretbg, alpha_caretfg, alpha_caretbg);
        draw.draw_rect({
            .x = static_cast<int>(x + curpos),
            .y = 2 + (ctx.item_height - fh) / 2 + y,
            .w = fw,
            .h = fh - 4,
        }, {
            .foreground = col_caretfg,
            .background = col_caretbg,
            .foreground_alpha = alpha_caretfg,
            .background_alpha = alpha_caretbg,
            .filled = true,
        });
    }

    return x;
}

int drawlarrow(int x, int y, int w) {
    if (hidelarrow) return x;

    if (currentitem->left) { // draw left arrow
        //draw_text(draw, x, y, w, sp.bh, sp.lrpad / 2, leftarrow, 0, pango_leftarrow ? True : False, col_larrowfg, col_larrowbg, alpha_larrowfg, alpha_larrowbg);
        draw.draw_text({
            .x = x,
            .y = y,
            .w = w,
            .h = ctx.item_height,
        }, ctx.lrpad / 2, leftarrow, pango_leftarrow, {
            .foreground = col_larrowfg,
            .background = col_larrowbg,
            .foreground_alpha = alpha_larrowfg,
            .background_alpha = alpha_larrowbg,
        });
        x += w;
    }

    return x;
}

int drawrarrow(int x, int y, int w) {
    if (hiderarrow) return x;

    if (nextitem) { // draw right arrow
        //draw_text(draw, sp.mw - w, y, w, sp.bh, sp.lrpad / 2, rightarrow, 0, pango_rightarrow ? True : False, col_rarrowfg, col_rarrowbg, alpha_rarrowfg, alpha_rarrowbg);
        draw.draw_text({
            .x = x,
            .y = y,
            .w = w,
            .h = ctx.item_height,
        }, ctx.lrpad / 2, rightarrow, pango_rightarrow, {
            .foreground = col_rarrowfg,
            .background = col_rarrowbg,
            .foreground_alpha = alpha_rarrowfg,
            .background_alpha = alpha_rarrowbg,
        });
        x += w;
    }

    return x;
}

int drawnumber(int x, int y, int w) {
    if (hidematchcount) return x;

    int powerlinewidth = 0;

    if (!hidepowerline && powerlinecount) {
        powerlinewidth = ctx.powerline_width / 2;
    }

    //draw_text(draw, x, y, w, sp.bh, sp.lrpad / 2 + powerlinewidth, tx.numbers, 0, pango_numbers ? True : False, col_numfg, col_numbg, alpha_numfg, alpha_numbg);
    draw.draw_text({
        .x = x,
        .y = y,
        .w = w,
        .h = ctx.item_height,
    }, ctx.lrpad / 2 + powerlinewidth, strings.number_text, pango_numbers, {
        .foreground = col_numfg,
        .background = col_numbg,
        .foreground_alpha = alpha_numfg,
        .background_alpha = alpha_numbg,
    });

    // draw powerline for match count
    if (!hidepowerline && powerlinecount) {
        if (matchcountpwlstyle == 2) {
            //draw_circle(draw, x, y, sp.plw, sp.bh, 0, col_menu, col_numbg, alpha_menu, alpha_numbg);
            draw.draw_circle({
                .x = x,
                .y = y,
                .w = ctx.powerline_width,
                .h = ctx.item_height,
            }, 0, {
                .prev = col_menu,
                .next = col_numbg,
                .prev_alpha = alpha_menu,
                .next_alpha = alpha_numbg,
            });
        } else {
            //draw_arrow(draw, x, y, sp.plw, sp.bh, 0, matchcountpwlstyle, col_menu, col_numbg, alpha_menu, alpha_numbg);
            draw.draw_arrow({
                .x = x,
                .y = y,
                .w = ctx.powerline_width,
                .h = ctx.item_height,
            }, 0, matchcountpwlstyle, {
                .prev = col_menu,
                .next = col_numbg,
                .prev_alpha = alpha_menu,
                .next_alpha = alpha_numbg,
            });
        }

        x += ctx.powerline_width;
    }

    return x;
}

int drawmode(int x, int y, int w) {
    if (!hidemode) { // draw mode indicator
        int powerlinewidth = 0;

        if (!hidepowerline && powerlinemode) {
            powerlinewidth = ctx.powerline_width / 2;
        }

        //draw_text(draw, x, y, w, sp.bh, sp.lrpad / 2 + powerlinewidth, tx.modetext, 0, pango_mode ? True : False, col_modefg, col_modebg, alpha_modefg, alpha_modebg);
        draw.draw_text({
            .x = x,
            .y = y,
            .w = w,
            .h = ctx.item_height,
        }, ctx.lrpad / 2 + powerlinewidth, strings.mode_text, pango_mode, {
            .foreground = col_modefg,
            .background = col_modebg,
            .foreground_alpha = alpha_modefg,
            .background_alpha = alpha_modebg,
        });

        // draw powerline for match count
        if (!hidepowerline && powerlinemode) {
            if (modepwlstyle == 2) {
                /*
                draw_circle(draw, x, y, sp.plw, sp.bh, 0,
                        hidematchcount ? col_menu : col_numbg, col_modebg,
                        hidematchcount ? alpha_menu : alpha_numbg, alpha_modebg);
                        */
                draw.draw_circle({
                    .x = x,
                    .y = y,
                    .w = ctx.powerline_width,
                    .h = ctx.item_height,
                }, 0, {
                    .prev = hidematchcount ? col_menu : col_numbg,
                    .next = col_modebg,
                    .prev_alpha = hidematchcount ? alpha_menu : alpha_numbg,
                    .next_alpha = alpha_modebg,
                });
            } else {
                /*
                draw_arrow(draw, x, y, sp.plw, sp.bh, 0, modepwlstyle,
                        hidematchcount ? col_menu : col_numbg, col_modebg,
                        hidematchcount ? alpha_menu : alpha_numbg, alpha_modebg);
                        */
                draw.draw_arrow({
                    .x = x,
                    .y = y,
                    .w = ctx.powerline_width,
                    .h = ctx.item_height,
                }, 0, modepwlstyle, {
                    .prev = hidemode ? hidematchcount ? col_menu : col_numbg : col_modebg,
                    .next = col_modebg,
                    .prev_alpha = hidemode ? hidematchcount ? alpha_menu : alpha_numbg : alpha_modebg,
                    .next_alpha = alpha_modebg,
                });
            }

            x += ctx.powerline_width;
        }
    }

    return x;
}

int drawcaps(int x, int y, int w) {
    if (!w) return x; // not caps lock

    if (!hidecaps) { // draw caps lock indicator
        int powerlinewidth = 0;

        if (!hidepowerline && powerlinecaps) {
            powerlinewidth = ctx.powerline_width / 2;
        }

        //draw_text(draw, x, y, w, sp.bh, sp.lrpad / 2 + powerlinewidth, tx.capstext, 0, pango_caps ? True : False, col_capsfg, col_capsbg, alpha_capsfg, alpha_capsbg);
        draw.draw_text({
            .x = x,
            .y = y,
            .w = w,
            .h = ctx.item_height,
        }, ctx.lrpad / 2 + powerlinewidth, strings.caps_text, pango_caps, {
            .foreground = col_capsfg,
            .background = col_capsbg,
            .foreground_alpha = alpha_capsfg,
            .background_alpha = alpha_capsbg,
        });

        // draw powerline for caps lock indicator
        if (!hidepowerline && powerlinecaps) {
            if (capspwlstyle == 2) {
                /*
                draw_circle(draw, x, y, sp.plw, sp.bh, 0,
                        hidemode ? hidematchcount ? col_menu : col_numbg : col_modebg, col_capsbg,
                        hidemode ? hidematchcount ? alpha_menu : alpha_numbg : alpha_modebg, alpha_capsbg);
                        */
                draw.draw_circle({
                    .x = x,
                    .y = y,
                    .w = ctx.powerline_width,
                    .h = ctx.item_height,
                }, 0, {
                    .prev = hidemode ? hidematchcount ? col_menu : col_numbg : col_modebg,
                    .next = col_capsbg,
                    .prev_alpha = hidemode ? hidematchcount ? alpha_menu : alpha_numbg : alpha_modebg,
                    .next_alpha = alpha_capsbg,
                });
            } else {
                /*
                draw_arrow(draw, x, y, sp.plw, sp.bh, 0, capspwlstyle,
                        hidemode ? hidematchcount ? col_menu : col_numbg : col_modebg, col_capsbg,
                        hidemode ? hidematchcount ? alpha_menu : alpha_numbg : alpha_modebg, alpha_capsbg);
                        */
                draw.draw_arrow({
                    .x = x,
                    .y = y,
                    .w = ctx.powerline_width,
                    .h = ctx.item_height,
                }, 0, capspwlstyle, {
                    .prev = hidemode ? hidematchcount ? col_menu : col_numbg : col_modebg,
                    .next = col_capsbg,
                    .prev_alpha = hidemode ? hidematchcount ? alpha_menu : alpha_numbg : alpha_modebg,
                    .next_alpha = alpha_capsbg,
                });
            }

            x += ctx.powerline_width;
        }
    }

    return x;
}

std::atomic_flag running_flag = ATOMIC_FLAG_INIT;
void drawmenu() {
    if (running_flag.test_and_set(std::memory_order_acquire))
        return;

    ctx.is_drawing = 1;
#if WAYLAND
    if (protocol) {
        if (!listfile.empty()) {
            readstdin();

            if (ctx.list_changed) {
                resizeclient();
                match();

                for (int i = 0; i < ctx.item_number; i++) {
                    if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
                        currentitem = nextitem;
                    }
                }
            }
        }

        drawmenu_layer();

#if IMAGE
        drawimage();
#endif

        commit_drawable(&state);
    } else {
#endif
#if X11
        if (!listfile.empty()) {
            readstdin();

            if (ctx.list_changed) {
                match();

                for (int i = 0; i < ctx.item_number; i++) {
                    if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
                        currentitem = nextitem;
                    }
                }
            }
        }

        drawmenu_layer();

#if IMAGE
        drawimage();
#endif
        draw.map(win);
#endif
#if WAYLAND
    }
#endif

    ctx.is_drawing = 0;
    running_flag.clear(std::memory_order_release);
}

void drawmenu_layer() {
    int x = 0, y = 0, w = 0;
    ctx.powerline_width = hidepowerline ? 0 : draw.get_font_manager().get_height() / 2 + 1; // powerline size

    sp_strncpy(strings.mode_text, ctx.mode ? instext.c_str() : normtext.c_str(), sizeof(strings.mode_text));

    if (regex && !regextext.empty() && ctx.mode) {
        sp_strncpy(strings.mode_text, regextext.c_str(), sizeof(strings.mode_text));
    }

    // draw menu first using menu scheme
    //draw_rect(draw, 0, 0, sp.mw, sp.mh, 1, 1, col_menu, col_menu, alpha_menu, alpha_menu);
    draw.draw_rect({
        .x = 0,
        .y = 0,
        .w = ctx.win_width,
        .h = ctx.win_height,
    }, {
        .foreground = col_menu,
        .background = col_menu,
        .foreground_alpha = alpha_menu,
        .background_alpha = alpha_menu,
        .filled = 1,
    });

    int numberw = 0;
    int modew = 0;
    int capsw = 0;

    // add width
    if (!hidemode) modew = pango_mode ? TEXTWM(strings.mode_text) : TEXTW(strings.mode_text);
    if (!hidecaps) capsw = pango_caps ? TEXTWM(strings.caps_text) : TEXTW(strings.caps_text);

    if (!strcmp(strings.caps_text, ""))
        capsw = 0;

    // calculate match count
    if (!hidematchcount) {
        recalculatenumbers();
        numberw = TEXTW(strings.number_text);
    }

    x += menumarginh;
    y += menumarginv;

    calcoffsets();
    get_mh();

    int nh = 1;

    // sp.bh must be removed from menu height resizing later
    if ((hideprompt && hideinput && hidemode && hidematchcount && hidecaps) && lines) {
        y -= ctx.item_height;
        nh = 0;
    }

    if (!hidemode) {
        modew = pango_mode ? TEXTWM(strings.mode_text) : TEXTW(strings.mode_text);
    }

    if (!hideprompt) {
        w = ctx.prompt_width;
        x = drawprompt(x, y + (nh ? lines ? itemposition ? (ctx.win_height - ctx.item_height) : 0 : 0 : 0), w);
    }

    w = (lines > 0 || !matches) ? ctx.win_width - x : ctx.input_width;
    x = drawinput(x, y + (nh ? lines ? itemposition ? (ctx.win_height - ctx.item_height) : 0 : 0 : 0), w);

    // draw the items, this function also calls drawrarrow() and drawlarrow()
    if (!hideitem) {
        drawitem(x, y - (nh ? lines ? itemposition ? ctx.item_height : 0 : 0 : 0), w);
    }

    if (!hidematchcount) {
        w = numberw;
        drawnumber(
                ctx.win_width - numberw - modew - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh,
                y + (nh ? lines ? itemposition ? (ctx.win_height - ctx.item_height) : 0 : 0 : 0),
                w
        );
    }

    if (!hidemode) {
        w = modew;
        drawmode(
                ctx.win_width - modew - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh,
                y + (nh ? lines ? itemposition ? (ctx.win_height - ctx.item_height) : 0 : 0 : 0),
                w
        );
    }

    if (!hidecaps) {
        w = capsw;
        drawcaps(
                ctx.win_width - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh,
                y + (nh ? lines ? itemposition ? (ctx.win_height - ctx.item_height) : 0 : 0 : 0),
                w
        );
    }
}

#if IMAGE
int draw_icon(item *item, int x, int y, int w, int h) {
    int ich = 0;
    int icw = 0;

    // cache
    int slen = 0, i;
    unsigned int digest_len = EVP_MD_size(EVP_md5());
    unsigned char *digest = (unsigned char *)OPENSSL_malloc(digest_len);
    char *xdg_cache, *home = nullptr, *cachesize, *buf = nullptr;
    struct passwd *user_id = nullptr;

    if (hideimage) {
        return x;
    }

    if (item->image) {
        if (generatecache) {
            if(!(xdg_cache = getenv("XDG_CACHE_HOME"))) {
                if(!(home = getenv("HOME")) && (user_id = getpwuid(getuid()))) {
                    home = user_id->pw_dir;
                }

                if(!home) { // no home directory.. somehow
                    fprintf(stderr, "majorna: could not find home directory");
                    return x;
                }
            }

            cachesize = "icon";

            slen = snprintf(nullptr, 0, "file://%s", item->image)+1;

            if(!(buf = static_cast<char*>(malloc(slen)))) {
                return x;
            }

            sprintf(buf, "file://%s", item->image);

            EVP_MD_CTX *mdcontext = EVP_MD_CTX_new();
            EVP_DigestInit_ex(mdcontext, EVP_md5(), nullptr);
            EVP_DigestUpdate(mdcontext, buf, slen);

            EVP_DigestFinal_ex(mdcontext, digest, &digest_len);
            EVP_MD_CTX_free(mdcontext);

            free(buf);

            char md5sum[digest_len*2+1];

            for (i = 0; i < digest_len; ++i)
                sprintf(&md5sum[i*2], "%02x", (unsigned int)digest[i]);

            if (cachedir.empty() || cachedir == "default") { // "default" here is from the config file
                if (xdg_cache || cachedir == "xdg")
                    slen = snprintf(nullptr, 0, "%s/thumbnails/%s/%s.png", xdg_cache, cachesize, md5sum)+1;
                else
                    slen = snprintf(nullptr, 0, "%s/.cache/thumbnails/%s/%s.png", home, cachesize, md5sum)+1;
            } else {
                slen = snprintf(nullptr, 0, "%s/%s/%s.png", cachedir, cachesize, md5sum)+1;
            }

            if(!(buf = static_cast<char*>(malloc(slen)))) {
                return x;
            }

            if (cachedir.empty() || cachedir == "default") { // "default" here is from the config file
                if (xdg_cache)
                    sprintf(buf, "%s/thumbnails/%s/%s.png", xdg_cache, cachesize, md5sum);
                else
                    sprintf(buf, "%s/.cache/thumbnails/%s/%s.png", home, cachesize, md5sum);
            } else {
                    sprintf(buf, "%s/%s/%s.png", cachedir, cachesize, md5sum);
            }

            image = imlib_load_image(buf); // attempt to load image from path

            if (image) {
                imlib_context_set_image(image);

                icw = imlib_image_get_width();
                ich = imlib_image_get_height();
            }
        }

        /* If passed w or h has changed, cache is invalid and has to be regenerated. This prevents flickering.
         * It could change if sp.bh has changed in some way, such as font size being changed after cache has
         * been generated.
         */
        if (ich != h || icw != w) {
            image = nullptr;
        }

        // load regular image, because an image was not loaded from cache
        if (!image || !generatecache) {
            image = imlib_load_image(item->image);

            if (!image) {
                return x;
            }

            imlib_context_set_image(image);

            icw = imlib_image_get_width();
            ich = imlib_image_get_height();

            if (!image) {
                return x;
            }

            image = imlib_create_cropped_scaled_image(0, 0, icw, ich, w, h); // w = h = bh - lrpad / 2
            imlib_free_image();
            imlib_context_set_image(image);

            // get new width/height
            icw = imlib_image_get_width();
            ich = imlib_image_get_height();
        }

        imlib_image_set_format("png");

        // create directory and write cache file
        if (buf && generatecache && image) {
            createifnexist_rec(buf);
            imlib_save_image(buf);
            free(buf);
        }

        // draw the image
        draw.draw_image(imlib_image_get_data(), {
            .x = x,
            .y = y,
            .w = w,
            .h = h,
        });

        x += w;
    }

    return x;
}
#endif
