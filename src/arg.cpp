#include <arg.hpp>
#include <options.hpp>
#include <majorna.hpp>
#include <draw.hpp>
#include <match.hpp>
#include <img.hpp>
#include <history.hpp>
#include <x11/clipboard.hpp>

void moveleft(const Arg& arg) {
    struct item *tmpsel;
    int i, offscreen = 0;
    int argu = arg.i ? arg.i : 1;

    // If we cannot move left because !lines, moving left should move to the next item. Calling moveup() does this.
    if (!lines) {
        moveup(arg);
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
                    drawmenu();
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

        drawmenu();
        calcoffsets();
    }
}

void moveright(const Arg& arg) {
    struct item *tmpsel;
    int i, offscreen = 0;
    int argu = arg.i ? arg.i : 1;

    if (!lines) { // If we cannot move right because !lines, moving right should move to the previous item. Calling down() does this.
        movedown(arg);
        return;
    }

    if (columns > 1) {
        if (!selecteditem)
            return;
        tmpsel = selecteditem;
        for (i = 0; i < lines; i++) {
            if (!tmpsel->right ||  tmpsel->right->left != tmpsel) {
                if (offscreen)
                    drawmenu();
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

    drawmenu();
}

void movedown(const Arg& arg) {
    int argu = arg.i ? arg.i : 1;

    for (int j = 0; j < argu; j++) {
        if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
            currentitem = nextitem; // Current item is now the next item
        }
    }

    calcoffsets();
    drawmenu();
}

void moveup(const Arg& arg) {
    int argu = arg.i ? arg.i : 1;

    for (int j = 0; j < argu; j++) {
        if (selecteditem && selecteditem->left && (selecteditem = selecteditem->left)->right == currentitem) {
            currentitem = previousitem; // Current item is now the previous item
        }
    }

    calcoffsets();
    drawmenu();
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

    strncpy(strings.text, selecteditem->nsgrtext, sizeof strings.text - 1);
    strings.text[sizeof strings.text - 1] = '\0';
    ctx.cursor = strlen(strings.text);

    match();
    drawmenu();
}

void movenext(const Arg& arg) {
    if (!nextitem) {
        return;
    }

    selecteditem = currentitem = nextitem; // next page
    drawmenu();
}

void moveprev(const Arg& arg) {
    if (!previousitem) {
        return;
    }

    selecteditem = currentitem = previousitem; // previous page
    calcoffsets();
    drawmenu();
}

void moveitem(const Arg& arg) {
    for (int i = 0; i < arg.i; i++) {
        if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
            currentitem = nextitem;
            calcoffsets();
        }
    }

    drawmenu();
}

void movestart(const Arg& arg) {
    if (selecteditem == matches) {
        ctx.cursor = 0;
        drawmenu();
        return;
    }

    selecteditem = currentitem = matches;
    calcoffsets();
    drawmenu();
}

void moveend(const Arg& arg) {
    if (strings.text[ctx.cursor] != '\0') {
        ctx.cursor = strlen(strings.text);
        drawmenu();
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
    drawmenu();
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

void viewhist(const Arg& arg) {
    int i;

    if (!histfile.empty()) {
        if (!history_items) {
            history_items = items;
            items = static_cast<item*>(calloc(histsz + 1, sizeof(struct item)));

            if (!items) {
                die("majorna: cannot allocate memory");
            }

            for (i = 0; i < histsz; i++) {
                items[i].text = history[i];
            }
        } else {
            free(items);
            items = history_items;
            history_items = nullptr;
        }
    }

    match();
    drawmenu();
}

void deleteword(const Arg& arg) {
    if (ctx.cursor == 0) return;

    while (ctx.cursor > 0 && strchr(worddelimiters.c_str(), strings.text[nextrune(-1)])) {
        insert(nullptr, nextrune(-1) - ctx.cursor);
    } while (ctx.cursor > 0 && !strchr(worddelimiters.c_str(), strings.text[nextrune(-1)])) {
        insert(nullptr, nextrune(-1) - ctx.cursor);
    }

    drawmenu();
}

void moveword(const Arg& arg) {
    if (arg.i < 0) { // move sp.cursor to the start of the word
        while (ctx.cursor > 0 && strchr(worddelimiters.c_str(), strings.text[nextrune(-1)])) {
            ctx.cursor = nextrune(-1);
        } while (ctx.cursor > 0 && !strchr(worddelimiters.c_str(), strings.text[nextrune(-1)])) {
            ctx.cursor = nextrune(-1);
        }
    } else { // move sp.cursor to the end of the word
        while (strings.text[ctx.cursor] && strchr(worddelimiters.c_str(), strings.text[ctx.cursor])) {
            ctx.cursor = nextrune(+1);
        } while (strings.text[ctx.cursor] && !strchr(worddelimiters.c_str(), strings.text[ctx.cursor])) {
            ctx.cursor = nextrune(+1);
        }
    }

    drawmenu();
}

void movecursor(const Arg& arg) {
    if (arg.i < 0) {
        if (ctx.cursor > 0) {
            ctx.cursor = nextrune(-1);
        }
    } else {
        if (strings.text[ctx.cursor]) {
            ctx.cursor = nextrune(+1);
        }
    }

    drawmenu();
}

void backspace(const Arg& arg) {
    if (ctx.cursor == 0)
        return;

    insert(nullptr, nextrune(-1) - ctx.cursor);
    drawmenu();
}

void markitem(const Arg& arg) {
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

void selectitem(const Arg& arg) {
    char *selection;

    // print index
    if (printindex && selecteditem && arg.i) {
        fprintf(stdout, "%d\n", selecteditem->index);
        cleanup();
        exit(0);
    }

    // selected item or input?
    if (selecteditem && arg.i && !hideitem) {
        selection = selecteditem->text;
    } else {
        selection = strings.text;
    }

    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] != -1 && (!selecteditem || selecteditem->index != sel_index[i])) {
            puts(items[sel_index[i]].text);
        }
    }

    if (!selection)
        return;

    puts(selection);
    savehistory(selection);

    cleanup();
    exit(0);
}

void navhistory(const Arg& arg) {
    navigatehistfile(arg.i);
    drawmenu();
}

void restoresel(const Arg& arg) {
    strings.text[ctx.cursor] = '\0';
    match();
    drawmenu();
}

void clear(const Arg& arg) {
    insert(nullptr, 0 - ctx.cursor);
    drawmenu();
}

void clearins(const Arg& arg) {
    insert(nullptr, 0 - ctx.cursor);

    ctx.mode = 1;
    ctx.allowkeys = 0;
    strncpy(strings.modetext, instext.c_str(), 15);

    calcoffsets();
    drawmenu();
}

void quit(const Arg& arg) {
    cleanup();
    exit(0);
}

void setlineheight(const Arg& arg) {
    lineheight += arg.i;
    ctx.bh = std::max(draw.get_font_manager().get_height(), draw.get_font_manager().get_height() + 2 + lineheight);

    resizeclient();
    drawmenu();
}

void setimgsize(const Arg& arg) {
#if IMAGE
    setimagesize(img.imagewidth + arg.i, img.imageheight + arg.i);
    drawmenu();
#endif
}

void flipimg(const Arg& arg) {
#if IMAGE

    if (!image) return;

    img.flip = img.flip ? 0 : arg.i ? 1 : 2;

    drawmenu();

#endif
}

void setimgpos(const Arg& arg) {
#if IMAGE
    if (!image || hideimage) return;

    if (imageposition < 3) {
        imageposition += arg.i;
    } else {
        imageposition = 0;
    }

    drawmenu();
#endif
}

void setimggaps(const Arg& arg) {
#if IMAGE
    img.imagegaps += arg.i;

    if (img.imagegaps < 0)
        img.imagegaps = 0;

    // limitation to make sure we have a reasonable gap size
    if (img.imagegaps > (ctx.mw - 2 * img.imagegaps) / 3)
        img.imagegaps -= arg.i;

    drawmenu();
#endif
}

void toggleinput(const Arg& arg) {
    hideinput = !hideinput;
    drawmenu();
}

void togglepretext(const Arg& arg) {
    hidepretext = !hidepretext;
    drawmenu();
}

void togglelarrow(const Arg& arg) {
    hidelarrow = !hidelarrow;
    drawmenu();
}

void togglerarrow(const Arg& arg) {
    hiderarrow = !hiderarrow;
    drawmenu();
}

void toggleitem(const Arg& arg) {
    hideitem = !hideitem;
    drawmenu();
}

void toggleprompt(const Arg& arg) {
    hideprompt = !hideprompt;
    drawmenu();
}

void togglecaps(const Arg& arg) {
    hidecaps = !hidecaps;
    drawmenu();
}

void togglepowerline(const Arg& arg) {
    hidepowerline = !hidepowerline;
    drawmenu();
}

void togglecaret(const Arg& arg) {
    hidecaret = !hidecaret;
    drawmenu();
}

void togglematchcount(const Arg& arg) {
    hidematchcount = !hidematchcount;
    drawmenu();
}

void togglemode(const Arg& arg) {
    hidemode = !hidemode;
    drawmenu();
}

void togglehighlight(const Arg& arg) {
    hidehighlight = !hidehighlight;
    drawmenu();
}

void toggleregex(const Arg& arg) {
#if REGEX
    regex = !regex;

    match();
    drawmenu();

#endif
}

void togglefuzzy(const Arg& arg) {
    fuzzy = !fuzzy;

    match();
    drawmenu();
}

void toggleimg(const Arg& arg) {
#if IMAGE

    hideimage = !hideimage;

    drawmenu();

#endif
}

void toggleimgtype(const Arg& arg) {
#if IMAGE
    imagetype = !imagetype;
#endif
}

void defaultimg(const Arg& arg) {
#if IMAGE

    if (hideimage || !image) return;

    img.imagewidth = imagewidth;
    img.imageheight = imageheight;
    img.imagegaps = imagegaps;

    drawmenu();
#endif
}

void setlines(const Arg& arg) {
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

    resizeclient();
    calcoffsets();
    drawmenu();
}

void setcolumns(const Arg& arg) {
    if (!overridecolumns || (hideprompt && hideinput && hidemode && hidematchcount && hidecaps)) return;

    columns += arg.i;

    if (columns < 1) {
        columns = 1;
    }

    if (lines == 0) {
        match();
    }

    resizeclient();
    calcoffsets();
    drawmenu();
}

void setx(const Arg& arg) {
    xpos += arg.i;

    resizeclient();
    drawmenu();
}

void sety(const Arg& arg) {
    ypos += arg.i;

    resizeclient();
    drawmenu();
}

void setw(const Arg& arg) {
    menuwidth += arg.i;

    resizeclient();
    drawmenu();
}

void spawn(const Arg& arg) {
    if (!system(arg.c))
        die("majorna: failed to execute command '%s'", arg.c);
    else
        exit(0);
}

void setprofile(const Arg& arg) {
    if (!system("command -v majorna_profile > /dev/null && majorna_profile --majorna-set-profile")) {
        die("majorna: failed to run profile menu\n");
    } else {
        exit(0);
    }
}

void switchmode(const Arg& arg) {
    if (forceinsertmode) {
        return;
    }

    ctx.mode = !ctx.mode;

    if (!type) ctx.mode = 0; // only normal mode allowed

    ctx.allowkeys = !ctx.mode;

    strncpy(strings.modetext, ctx.mode ? instext.c_str() : normtext.c_str(), 15);
    drawmenu();
}

/* This function is basically a copy of the selectitem function.
 * The only difference is "selectitem" was replaced with "mouseitem" and tx.text output
 * was removed.
 */
void outputhover(const Arg& arg) {
    char *selection;

    if (printindex && mouseitem && arg.i) {
        fprintf(stdout, "%d\n", mouseitem->index);
        cleanup();
        exit(0);
    }

    selection = mouseitem->text;

    for (int i = 0; i < sel_size; i++) {
        if (sel_index[i] != -1 && (!mouseitem || mouseitem->index != sel_index[i])) {
            puts(items[sel_index[i]].text);
        }
    }

    if (!selection)
        return;

    puts(selection);
    savehistory(selection);

    cleanup();
    exit(0);
}

void selecthover(const Arg& arg) {
    if (selecteditem != mouseitem) {
        selecteditem = mouseitem;
    } else {
        selecteditem = mouseitem;
        outputhover(arg);

        return;
    }

    drawmenu();
}

void markhover(const Arg& arg) {
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

    drawmenu();
}

void screenshot(const Arg& arg) {
    char *file = nullptr;
    char *home = nullptr;
    time_t time_ = time(nullptr);
    struct tm t = *localtime(&time_);

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
            if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s/%s", home, screenshotname) + 1)))) {
                die("majorna: failed to malloc screenshot file");
            }

            sprintf(file, "%s/%s", home, screenshotname);
        } else if (!screenshotdir.empty() && screenshotname.empty()) { // dir but no name
            if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s/%s-%02d-%02d-%02d%s", screenshotdir, "majorna-screenshot", t.tm_hour, t.tm_min, t.tm_sec, ".png") + 1)))) {
                die("majorna: failed to malloc screenshot file");
            }

            sprintf(file, "%s/%s-%02d-%02d-%02d%s", screenshotdir, "majorna-screenshot", t.tm_hour, t.tm_min, t.tm_sec, ".png");
        } else { // dir and name
            if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s/%s", screenshotdir, screenshotname) + 1)))) {
                die("majorna: failed to malloc screenshot file");
            }

            sprintf(file, "%s/%s", screenshotdir, screenshotname);
        }
    } else { // custom file
        if (!(file = static_cast<char*>(malloc(snprintf(nullptr, 0, "%s", screenshotfile) + 1)))) {
            die("majorna: failed to malloc screenshot file");
        }

        sprintf(file, "%s", screenshotfile);
    }

    draw.save_screen(file);

    if (file) {
        free(file);
    }
}
