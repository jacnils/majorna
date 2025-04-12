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
void moveup(Arg *arg);
void movedown(Arg *arg);
void moveleft(Arg *arg);
void moveright(Arg *arg);
void moveend(Arg *arg);
void movestart(Arg *arg);
void movenext(Arg *arg);
void moveprev(Arg *arg);
void moveitem(Arg *arg);
void moveword(Arg *arg);
void movecursor(Arg *arg);

/* input */
void paste(Arg *arg);
void restoresel(Arg *arg);
void clear(Arg *arg);
void clearins(Arg *arg);
void deleteword(Arg *arg);
void backspace(Arg *arg);
void complete(Arg *arg);

/* history */
void viewhist(Arg *arg);
void navhistory(Arg *arg);

/* selection */
void markitem(Arg *arg);
void selectitem(Arg *arg);

/* image */
void setimgsize(Arg *arg);
void defaultimg(Arg *arg);
void flipimg(Arg *arg);
void setimgpos(Arg *arg);
void setimggaps(Arg *arg);

/* lines/columns */
void setlines(Arg *arg);
void setcolumns(Arg *arg);

/* misc */
void spawn(Arg *arg);
void screenshot(Arg *arg);
void switchmode(Arg *arg);
void setprofile(Arg *arg);
void setlineheight(Arg *arg);
void setx(Arg *arg);
void sety(Arg *arg);
void setw(Arg *arg);
void quit(Arg *arg);

/* toggle */
void toggleinput(Arg *arg);
void togglepretext(Arg *arg);
void togglelarrow(Arg *arg);
void togglerarrow(Arg *arg);
void toggleitem(Arg *arg);
void toggleprompt(Arg *arg);
void togglecaps(Arg *arg);
void togglepowerline(Arg *arg);
void togglecaret(Arg *arg);
void togglematchcount(Arg *arg);
void togglemode(Arg *arg);
void togglehighlight(Arg *arg);
void toggleregex(Arg *arg);
void togglefuzzy(Arg *arg);
void toggleimg(Arg *arg);
void toggleimgtype(Arg *arg);

/* hover */
void selecthover(Arg *arg);
void outputhover(Arg *arg);
void markhover(Arg *arg);
