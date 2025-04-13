#pragma once

struct Arg {
    int i{};
    unsigned int ui{};
    float f{};
    const void *v{};
    const char *c{};
};

/* declare keybind functions
 *
 * movement
 */
void moveup(const Arg& arg);
void movedown(const Arg& arg);
void moveleft(const Arg& arg);
void moveright(const Arg& arg);
void moveend(const Arg& arg);
void movestart(const Arg& arg);
void movenext(const Arg& arg);
void moveprev(const Arg& arg);
void moveitem(const Arg& arg);
void moveword(const Arg& arg);
void movecursor(const Arg& arg);

/* input */
void paste(const Arg& arg);
void restoresel(const Arg& arg);
void clear(const Arg& arg);
void clearins(const Arg& arg);
void deleteword(const Arg& arg);
void backspace(const Arg& arg);
void complete(const Arg& arg);

/* history */
void viewhist(const Arg& arg);
void navhistory(const Arg& arg);

/* selection */
void markitem(const Arg& arg);
void selectitem(const Arg& arg);

/* image */
void setimgsize(const Arg& arg);
void defaultimg(const Arg& arg);
void flipimg(const Arg& arg);
void setimgpos(const Arg& arg);
void setimggaps(const Arg& arg);

/* lines/columns */
void setlines(const Arg& arg);
void setcolumns(const Arg& arg);

/* misc */
void spawn(const Arg& arg);
void screenshot(const Arg& arg);
void switchmode(const Arg& arg);
void setprofile(const Arg& arg);
void setlineheight(const Arg& arg);
void setx(const Arg& arg);
void sety(const Arg& arg);
void setw(const Arg& arg);
void quit(const Arg& arg);

/* toggle */
void toggleinput(const Arg& arg);
void togglepretext(const Arg& arg);
void togglelarrow(const Arg& arg);
void togglerarrow(const Arg& arg);
void toggleitem(const Arg& arg);
void toggleprompt(const Arg& arg);
void togglecaps(const Arg& arg);
void togglepowerline(const Arg& arg);
void togglecaret(const Arg& arg);
void togglematchcount(const Arg& arg);
void togglemode(const Arg& arg);
void togglehighlight(const Arg& arg);
void toggleregex(const Arg& arg);
void togglefuzzy(const Arg& arg);
void toggleimg(const Arg& arg);
void toggleimgtype(const Arg& arg);

/* hover */
void selecthover(const Arg& arg);
void outputhover(const Arg& arg);
void markhover(const Arg& arg);
