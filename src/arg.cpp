#include <arg.hpp>
#include <options.hpp>
#include <majorna.hpp>
#include <draw.hpp>
#include <match.hpp>
#include <img.hpp>
#include <history.hpp>
#include <x11/clipboard.hpp>

void move_left(const Arg& arg) {
    struct item *tmpsel;
    int i, offscreen = 0;
    int argu = arg.i ? arg.i : 1;

    // If we cannot move left because !lines, moving left should move to the next item. Calling move_up() does this.
    if (!lines) {
        move_up(arg);
        return;
    }

    if (columns > 1) {
        if (!selecteditem) {
            return;
        }

        tmpsel = selecteditem;
        for (i = 0; i < lines; i++) {
            if (!tmpsel->left || tmpsel->left->right != tmpsel) {
                if (offscreen)
                    draw_menu();
                return;
            }
            if (tmpsel == currentitem)
                offscreen = 1;
            tmpsel = tmpsel->left;
        }
        selecteditem = tmpsel;
        if (offscreen) {
            for (int j = 0; j < argu; j++) {
                currentitem = previousitem;
            }
        }

        draw_menu();
        calcoffsets();
    }
}

void move_right(const Arg& arg) {
    struct item *tmpsel;
    int i, offscreen = 0;
    int argu = arg.i ? arg.i : 1;

    if (!lines) { // If we cannot move right because !lines, moving right should move to the previous item. Calling down() does this.
        move_down(arg);
        return;
    }

    if (columns > 1) {
        if (!selecteditem)
            return;
        tmpsel = selecteditem;
        for (i = 0; i < lines; i++) {
            if (!tmpsel->right ||  tmpsel->right->left != tmpsel) {
                if (offscreen)
                    draw_menu();
                return;
            }
            tmpsel = tmpsel->right;
            if (tmpsel == nextitem)
                offscreen = 1;
        }
        selecteditem = tmpsel;
        if (offscreen) {
            for (int j = 0; j < argu; j++)
                currentitem = nextitem;
        }
        calcoffsets();
    }

    draw_menu();
}

void move_down(const Arg& arg) {
    int argu = arg.i ? arg.i : 1;

    for (int j = 0; j < argu; j++) {
        if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
            currentitem = nextitem; // Current item is now the next item
        }
    }

    calcoffsets();
    draw_menu();
}

void move_up(const Arg& arg) {
    int argu = arg.i ? arg.i : 1;

    for (int j = 0; j < argu; j++) {
        if (selecteditem && selecteditem->left && (selecteditem = selecteditem->left)->right == currentitem) {
            currentitem = previousitem; // Current item is now the previous item
        }
    }

    calcoffsets();
    draw_menu();
}

void complete(const Arg& arg) {
    int itc = 0;
    struct item *item;

    if (hideitem) return;

    if (matchend) {
        itc++;

        for (item = matchend; item && item->left; item = item->left)
            itc++;
    }

    if (!itc) {
        return;
    }

    strncpy(strings.input_text, selecteditem->text_without_sequences, sizeof strings.input_text - 1);
    strings.input_text[sizeof strings.input_text - 1] = '\0';
    ctx.cursor = strlen(strings.input_text);

    match();
    draw_menu();
}

void move_next(const Arg& arg) {
    if (!nextitem) {
        return;
    }

    selecteditem = currentitem = nextitem; // next page
    draw_menu();
}

void move_prev(const Arg& arg) {
    if (!previousitem) {
        return;
    }

    selecteditem = currentitem = previousitem; // previous page
    calcoffsets();
    draw_menu();
}

void move_item(const Arg& arg) {
    for (int i = 0; i < arg.i; i++) {
        if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
            currentitem = nextitem;
            calcoffsets();
        }
    }

    draw_menu();
}

void move_start(const Arg& arg) {
    if (selecteditem == matches) {
        ctx.cursor = 0;
        draw_menu();
        return;
    }

    selecteditem = currentitem = matches;
    calcoffsets();
    draw_menu();
}

void move_end(const Arg& arg) {
    if (strings.input_text[ctx.cursor] != '\0') {
        ctx.cursor = strlen(strings.input_text);
        draw_menu();
        return;
    }

    if (nextitem) {
        currentitem = matchend;
        calcoffsets();
        currentitem = previousitem;
        calcoffsets();

        while (nextitem && (currentitem = currentitem->right))
            calcoffsets();
    }

    selecteditem = matchend;
    draw_menu();
}

void paste(const Arg& arg) {
#if WAYLAND
    if (protocol) {
        paste_wl();
    } else {
#if X11
        paste_x11(arg.i);
#endif
    }
#elif X11
    paste_x11(arg.i);
#endif
}

void view_history(const Arg& arg) {
    int i;

    if (!histfile.empty()) {
        if (!history_items) {
            history_items = items;
            items = static_cast<item*>(calloc(histsz + 1, sizeof(struct item)));

            if (!items) {
                die("majorna: cannot allocate memory");
            }

            for (i = 0; i < histsz; i++) {
                items[i].raw_text = history[i];
            }
        } else {
            free(items);
            items = history_items;
            history_items = nullptr;
        }
    }

    match();
    draw_menu();
}

void delete_word(const Arg& arg) {
    if (ctx.cursor == 0) return;

    while (ctx.cursor > 0 && strchr(worddelimiters.c_str(), strings.input_text[nextrune(-1)])) {
        insert(nullptr, nextrune(-1) - ctx.cursor);
    } while (ctx.cursor > 0 && !strchr(worddelimiters.c_str(), strings.input_text[nextrune(-1)])) {
        insert(nullptr, nextrune(-1) - ctx.cursor);
    }

    draw_menu();
}

void move_word(const Arg& arg) {
    if (arg.i < 0) { // move sp.cursor to the start of the word
        while (ctx.cursor > 0 && strchr(worddelimiters.c_str(), strings.input_text[nextrune(-1)])) {
            ctx.cursor = nextrune(-1);
        } while (ctx.cursor > 0 && !strchr(worddelimiters.c_str(), strings.input_text[nextrune(-1)])) {
            ctx.cursor = nextrune(-1);
        }
    } else { // move sp.cursor to the end of the word
        while (strings.input_text[ctx.cursor] && strchr(worddelimiters.c_str(), strings.input_text[ctx.cursor])) {
            ctx.cursor = nextrune(+1);
        } while (strings.input_text[ctx.cursor] && !strchr(worddelimiters.c_str(), strings.input_text[ctx.cursor])) {
            ctx.cursor = nextrune(+1);
        }
    }

    draw_menu();
}

void move_cursor(const Arg& arg) {
    if (arg.i < 0) {
        if (ctx.cursor > 0) {
            ctx.cursor = nextrune(-1);
        }
    } else {
        if (strings.input_text[ctx.cursor]) {
            ctx.cursor = nextrune(+1);
        }
    }

    draw_menu();
}

void backspace(const Arg& arg) {
    if (ctx.cursor == 0)
        return;

    insert(nullptr, nextrune(-1) - ctx.cursor);
    draw_menu();
}

void mark_item(const Arg& arg) {
    if (!mark) return;
    if (selecteditem && is_selected(selecteditem->index)) {
        for (int i = 0; i < sel_size; i++) {
            if (sel_index[i] == selecteditem->index) {
                sel_index[i] = -1;
            }
        }
    } else {
        for (int i = 0; i < sel_size; i++) {
            if (sel_index[i] == -1) {
                sel_index[i] = selecteditem->index;
                return;
            }
        }

        sel_size++;
        sel_index = static_cast<int*>(realloc(sel_index, (sel_size + 1) * sizeof(int)));
        sel_index[sel_size - 1] = selecteditem->index;
    }
}

void select_item(const Arg& arg) {
    char *selection;

    // print index
    if (printindex && selecteditem && arg.i) {
        fprintf(stdout, "%d\n", selecteditem->index);
        cleanup();
        exit(0);
    }

    // selected item or input?
    if (selecteditem && arg.i && !hideitem) {
        selection = selecteditem->raw_text;
    } else {
        selection = strings.input_text;
    }

    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] != -1 && (!selecteditem || selecteditem->index != sel_index[i])) {
            puts(items[sel_index[i]].raw_text);
        }
    }

    if (!selection)
        return;

    puts(selection);
    savehistory(selection);

    cleanup();
    exit(0);
}

void navigate_history(const Arg& arg) {
    navigatehistfile(arg.i);
    draw_menu();
}

void restore_selection(const Arg& arg) {
    strings.input_text[ctx.cursor] = '\0';
    match();
    draw_menu();
}

void clear(const Arg& arg) {
    insert(nullptr, 0 - ctx.cursor);
    draw_menu();
}

void clear_and_insert(const Arg& arg) {
    insert(nullptr, 0 - ctx.cursor);

    ctx.mode = 1;
    ctx.allow_input = 0;
    strncpy(strings.mode_text, instext.c_str(), 15);

    calcoffsets();
    draw_menu();
}

void quit(const Arg& arg) {
    cleanup();
    exit(0);
}

void set_line_height(const Arg& arg) {
    lineheight += arg.i;
    ctx.item_height = std::max(draw.get_font_manager().get_height(), draw.get_font_manager().get_height() + 2 + lineheight);

    resize_client();
    draw_menu();
}

void set_image_size(const Arg& arg) {
#if IMAGE
    setimagesize(img.width + arg.i, img.height + arg.i);
    draw_menu();
#endif
}

void flip_image(const Arg& arg) {
#if IMAGE

    if (!image) return;

    img.flip = img.flip ? 0 : arg.i ? 1 : 2;

    draw_menu();

#endif
}

void set_image_position(const Arg& arg) {
#if IMAGE
    if (!image || hideimage) return;

    if (imageposition < 3) {
        imageposition += arg.i;
    } else {
        imageposition = 0;
    }

    draw_menu();
#endif
}

void set_image_gaps(const Arg& arg) {
#if IMAGE
    img.gaps += arg.i;

    if (img.gaps < 0)
        img.gaps = 0;

    // limitation to make sure we have a reasonable gap size
    if (img.gaps > (ctx.win_width - 2 * img.gaps) / 3)
        img.gaps -= arg.i;

    draw_menu();
#endif
}

void toggle_input(const Arg& arg) {
    hideinput = !hideinput;
    draw_menu();
}

void toggle_pretext(const Arg& arg) {
    hidepretext = !hidepretext;
    draw_menu();
}

void toggle_left_arrow(const Arg& arg) {
    hidelarrow = !hidelarrow;
    draw_menu();
}

void toggle_right_arrow(const Arg& arg) {
    hiderarrow = !hiderarrow;
    draw_menu();
}

void toggle_item(const Arg& arg) {
    hideitem = !hideitem;
    draw_menu();
}

void toggle_prompt(const Arg& arg) {
    hideprompt = !hideprompt;
    draw_menu();
}

void toggle_caps(const Arg& arg) {
    hidecaps = !hidecaps;
    draw_menu();
}

void toggle_powerline(const Arg& arg) {
    hidepowerline = !hidepowerline;
    draw_menu();
}

void toggle_caret(const Arg& arg) {
    hidecaret = !hidecaret;
    draw_menu();
}

void toggle_match_count(const Arg& arg) {
    hidematchcount = !hidematchcount;
    draw_menu();
}

void toggle_mode_indicator(const Arg& arg) {
    hidemode = !hidemode;
    draw_menu();
}

void toggle_highlight(const Arg& arg) {
    hidehighlight = !hidehighlight;
    draw_menu();
}

void toggle_regex(const Arg& arg) {
    regex = !regex;

    match();
    draw_menu();
}

void toggle_fuzzy_matching(const Arg& arg) {
    fuzzy = !fuzzy;

    match();
    draw_menu();
}

void toggle_image(const Arg& arg) {
#if IMAGE

    hideimage = !hideimage;

    draw_menu();

#endif
}

void toggle_image_type(const Arg& arg) {
#if IMAGE
    imagetype = !imagetype;
#endif
}

void reset_image(const Arg& arg) {
#if IMAGE

    if (hideimage || !image) return;

    img.width = imagewidth;
    img.height = imageheight;
    img.gaps = imagegaps;

    draw_menu();
#endif
}

void set_lines(const Arg& arg) {
    if (!overridelines || (hideprompt && hideinput && hidemode && hidematchcount && hidecaps)) return;

    insert(nullptr, 0 - ctx.cursor);
    selecteditem = currentitem = matches;

    if (lines + arg.i >= minlines) {
        lines += arg.i;
    }

    if (lines < 0) {
        lines = 0;
    }

    if (lines == 0) {
        match();
    }

    resize_client();
    calcoffsets();
    draw_menu();
}

void set_columns(const Arg& arg) {
    if (!overridecolumns || (hideprompt && hideinput && hidemode && hidematchcount && hidecaps)) return;

    columns += arg.i;

    if (columns < 1) {
        columns = 1;
    }

    if (lines == 0) {
        match();
    }

    resize_client();
    calcoffsets();
    draw_menu();
}

void set_x(const Arg& arg) {
    xpos += arg.i;

    resize_client();
    draw_menu();
}

void set_y(const Arg& arg) {
    ypos += arg.i;

    resize_client();
    draw_menu();
}

void set_w(const Arg& arg) {
    menuwidth += arg.i;

    resize_client();
    draw_menu();
}

void spawn(const Arg& arg) {
    if (!system(arg.c.c_str()))
        die("majorna: failed to execute command '%s'", arg.c.c_str());
    else
        exit(0);
}

void set_profile(const Arg& arg) {
    if (!system("command -v majorna_profile > /dev/null && majorna_profile --majorna-set-profile")) {
        die("majorna: failed to run profile menu\n");
    } else {
        exit(0);
    }
}

void toggle_mode(const Arg& arg) {
    if (forceinsertmode) {
        return;
    }

    ctx.mode = !ctx.mode;

    if (!type) ctx.mode = 0; // only normal mode allowed

    ctx.allow_input = !ctx.mode;

    strncpy(strings.mode_text, ctx.mode ? instext.c_str() : normtext.c_str(), 15);
    draw_menu();
}

/* This function is basically a copy of the select_item function.
 * The only difference is "select_item" was replaced with "mouseitem" and tx.text output
 * was removed.
 */
void output_hover(const Arg& arg) {
    char *selection;

    if (printindex && mouseitem && arg.i) {
        fprintf(stdout, "%d\n", mouseitem->index);
        cleanup();
        exit(0);
    }

    selection = mouseitem->raw_text;

    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] != -1 && (!mouseitem || mouseitem->index != sel_index[i])) {
            puts(items[sel_index[i]].raw_text);
        }
    }

    if (!selection)
        return;

    puts(selection);
    savehistory(selection);

    cleanup();
    exit(0);
}

void select_hover(const Arg& arg) {
    if (selecteditem != mouseitem) {
        selecteditem = mouseitem;
    } else {
        selecteditem = mouseitem;
        output_hover(arg);

        return;
    }

    draw_menu();
}

void mark_hover(const Arg& arg) {
    if (!mark) return;
    if (mouseitem && is_selected(mouseitem->index)) {
        for (int i = 0; i < sel_size; i++) {
            if (sel_index[i] == mouseitem->index) {
                sel_index[i] = -1;
            }
        }
    } else {
        for (int i = 0; i < sel_size; i++) {
            if (sel_index[i] == -1) {
                sel_index[i] = mouseitem->index;
                return;
            }
        }

        sel_size++;
        sel_index = static_cast<int*>(realloc(sel_index, (sel_size + 1) * sizeof(int)));
        sel_index[sel_size - 1] = mouseitem->index;
    }

    draw_menu();
}

void screenshot(const Arg& arg) {
    char *file = nullptr;
    char *home = nullptr;
    time_t time_ = time(nullptr);
    struct tm t = *localtime(&time_);

    /*
    if (screenshotfile.empty()) {
        if (!(home = getenv("HOME"))) {
            fprintf(stderr, "majorna: failed to determine home directory\n");
            return;
        }

        if (screenshotdir.empty() && screenshotname.empty()) { // default
            if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s/%s-%02d-%02d-%02d%s", home, "majorna-screenshot", t.tm_hour, t.tm_min, t.tm_sec, ".png") + 1)))) {
                die("majorna: failed to malloc screenshot file");
            }

            sprintf(file, "%s/%s-%02d-%02d-%02d%s", home, "majorna-screenshot", t.tm_hour, t.tm_min, t.tm_sec, ".png");
        } else if (screenshotdir.empty() && !screenshotname.empty()) { // no dir but name
            if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s/%s", home, screenshotname.c_str()) + 1)))) {
                die("majorna: failed to malloc screenshot file");
            }

            sprintf(file, "%s/%s", home, screenshotname.c_str());
        } else if (!screenshotdir.empty() && screenshotname.empty()) { // dir but no name
            if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s/%s-%02d-%02d-%02d%s", screenshotdir.c_str(), "majorna-screenshot", t.tm_hour, t.tm_min, t.tm_sec, ".png") + 1)))) {
                die("majorna: failed to malloc screenshot file");
            }

            sprintf(file, "%s/%s-%02d-%02d-%02d%s", screenshotdir.c_str(), "majorna-screenshot", t.tm_hour, t.tm_min, t.tm_sec, ".png");
        } else { // dir and name
            if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s/%s", screenshotdir.c_str(), screenshotname.c_str()) + 1)))) {
                die("majorna: failed to malloc screenshot file");
            }

            sprintf(file, "%s/%s", screenshotdir.c_str(), screenshotname.c_str());
        }
    } else { // custom file
        if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s", screenshotfile.c_str()) + 1)))) {
            die("majorna: failed to malloc screenshot file");
        }

        sprintf(file, "%s", screenshotfile.c_str());
    }
    */

    draw.save_screen(file);

    if (file) {
        free(file);
    }
}
