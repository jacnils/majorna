#pragma once

#include <cstddef>
#include <cstdio>
#include <macros.hpp>
#include <limhamn/primitive/primitive.hpp>
#include <options.hpp>
#include <x11/x11_libs.hpp>
#include <x11/key.hpp>
#include <x11/mouse.hpp>
#include <wl/wayland.hpp>

enum {
    ClickWindow,
    ClickPrompt,
    ClickInput,
    ClickLArrow,
    ClickItem,
    ClickRArrow,
    ClickNumber,
    ClickCaps,
    ClickMode,
    ClickImage,
    ClickNone,
};

struct item {
    char *text;
    char *nsgrtext;
    char *image;
    char *ex;
    item* left, *right;
    int hp;
    int index;
    double distance;
};

struct sp {
    int bh; // height of each menu item
    int mw; // width
    int mh; // height
    int vp; // vertical padding for bar
    int sp; // side padding for bar
    int lrpad; // sum of left and right padding

    int mode; // current mode
    int allowkeys; // interpret a keypress as an insertion?
    int capslockstate; // caps lock state
    int isdrawing;

    int inputw; // input width
    int promptw; // prompt width
    int plw; // powerline width

    int itemnumber; // item number
    int listcount;
    int listchanged;

    int maxlen; // max length of text

    size_t cursor; // cursor width

    int ignoreconfkeys; // can be set globally if you don't want to override keybinds with config file keys
    int ignoreglobalkeys; // should be set in the config file, if 1, the Keys keys array is ignored
    int ignoreconfmouse; // same for mouse
    int ignoreglobalmouse; // same for mouse
};

struct mo {
    int output_width; // output width
    int output_height; // output height
    int output_xpos; // output x position
    int output_ypos; // output y position
};

#if IMAGE
struct img {
    int setlines; // actual lines
    int flip; // %=
    int longestedge; // MAX(imagewidth, imageheight)
    int imagewidth; // current image width
    int imageheight; // current image height
    int imagegaps; // current image gaps
};
#endif

struct tx {
    char modetext[64]; // mode text
    char text[BUFSIZ]; // input text
    char numbers[NUMBERSBUFSIZE]; // number text
    char capstext[64]; // caps lock text
};

#if X11
struct x11 {
    int numlockmask;
    int useargb;
    int depth;
    char *embed;
    int screen;
    Visual *visual;
    Colormap cmap;
};
#endif

inline sp sp = {0};
inline tx tx = {0};
inline mo mo = {0};
#if IMAGE
inline img img = {0};
#endif
#if X11
inline x11 x11 = {0};
#endif

inline item *items = nullptr;
inline item *history_items;
inline item *list_items;
inline item *matches;
inline item *matchend;

inline item *previousitem;
inline item *currentitem;
inline item *nextitem;
inline item *selecteditem;
inline item *mouseitem;

inline limhamn::primitive::draw_manager draw;

inline int hplength = 0;
inline char **hpitems = nullptr;

inline int theme_override = 0;
inline int binds_override = 0;
inline int protocol_override = 0;

inline int *sel_index = nullptr;
inline unsigned int sel_size = 0;
inline int itemn = 0;

#if RTL
inline int isrtl = 1;
#else
inline int isrtl = 0;
#endif

int is_selected(size_t index);
void calcoffsets(void);
void recalculatenumbers(void);
void insert(const char *str, ssize_t n);
void cleanup(void);
void resizeclient(void);
void get_width(void);
void get_mh(void);
void set_mode(void);
void handle(void);
void appenditem(struct item *item, struct item **list, struct item **last);
int max_textw(void);
size_t nextrune(int inc);

char * cistrstr(const char *s, const char *sub);
inline int (*fstrncmp)(const char *, const char *, size_t) = strncasecmp;
inline char *(*fstrstr)(const char *, const char *) = cistrstr;

inline char **list;
inline size_t listsize;
inline char* fonts[] = {font};

#if X11
inline Key keys[] = {
    { static_cast<unsigned int>(-1),      0,              XK_Return,    selectitem,      {.i = +1 } },
    { static_cast<unsigned int>(-1),      Shift,          XK_Return,    selectitem,      {0} },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_Return,    markitem,        {0} },
    { static_cast<unsigned int>(-1),      0,              XK_Tab,       complete,        {0} },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_v,         paste,           {.i = 2  } },
    { static_cast<unsigned int>(-1),      0,              XK_BackSpace, backspace,       {0} },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_BackSpace, deleteword,      {0} },
    { static_cast<unsigned int>(-1),      Ctrl|Shift,     XK_p,         setprofile,      {0} },
    { static_cast<unsigned int>(-1),      0,              XK_Print,     screenshot,      {0} },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_equal,     setimgsize,      {.i = +10 } },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_minus,     setimgsize,      {.i = -10 } },
    { static_cast<unsigned int>(-1),      0,              XK_Up,        moveup,          {0} },
    { static_cast<unsigned int>(-1),      0,              XK_Down,      movedown,        {0} },
    { static_cast<unsigned int>(-1),      0,              XK_Left,      moveleft,        {0} },
    { static_cast<unsigned int>(-1),      0,              XK_Right,     moveright,       {0} },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_u,         moveup,          {.i = 5  } },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_d,         movedown,        {.i = 5  } },
    { static_cast<unsigned int>(-1),      Shift,          XK_h,         viewhist,        {0} },
    { static_cast<unsigned int>(-1),      0,              XK_Escape,    quit,            {0} },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_p,         navhistory,      {.i = -1 } },
    { static_cast<unsigned int>(-1),      Ctrl,           XK_n,         navhistory,      {.i = +1 } },
};
#endif
#if WAYLAND
inline WlKey wl_keys[] = {
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Return,    selectitem,      {.i = +1 } },
    { static_cast<unsigned int>(-1),      WL_Shift,             XKB_KEY_Return,    selectitem,      {0} },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_Return,    markitem,        {0} },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Tab,       complete,        {0} },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_v,         paste,           {.i = 2  } },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_BackSpace, backspace,       {0} },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_BackSpace, deleteword,      {0} },
    { static_cast<unsigned int>(-1),      WL_CtrlShift,         XKB_KEY_p,         setprofile,      {0} },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Print,     screenshot,      {0} },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_equal,     setimgsize,      {.i = +10 } },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_minus,     setimgsize,      {.i = -10 } },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Up,        moveup,          {0} },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Down,      movedown,        {0} },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Left,      moveleft,        {0} },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Right,     moveright,       {0} },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_u,         moveup,          {.i = 5  } },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_d,         movedown,        {.i = 5  } },
    { static_cast<unsigned int>(-1),      WL_Shift,             XKB_KEY_h,         viewhist,        {0} },
    { static_cast<unsigned int>(-1),      WL_None,              XKB_KEY_Escape,    quit,            {0} },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_p,         navhistory,      {.i = -1 } },
    { static_cast<unsigned int>(-1),      WL_Ctrl,              XKB_KEY_n,         navhistory,      {.i = +1 } },

};
#endif
#if X11
inline Mouse buttons[] = {
    { ClickInput,           Button1,         clear,        {0} },
    { ClickPrompt,          Button1,         clear,        {0} },
    { ClickMode,            Button1,         switchmode,   {0} },
    { ClickNumber,          Button1,         viewhist,     {0} },
    { ClickItem,            Button1,         selecthover,  {0} },
    { ClickItem,            Button2,         markhover,    {0} },
    { ClickNone,            Button5,         movenext,     {0} },
    { ClickNone,            Button4,         moveprev,     {0} },
};
#endif
#if WAYLAND
inline WlMouse wl_buttons[] = {
    { ClickInput,           WL_Left,         clear,        {0} },
    { ClickPrompt,          WL_Left,         clear,        {0} },
    { ClickMode,            WL_Left,         switchmode,   {0} },
    { ClickNumber,          WL_Left,         viewhist,     {0} },
    { ClickItem,            WL_Left,         selecthover,  {0} },
    { ClickItem,            WL_Right,        markhover,    {0} },
    { ClickNone,            WL_Down,         movenext,     {0} },
    { ClickNone,            WL_Up,           moveprev,     {0} },
};
#endif

int is_selected(size_t index);
void appenditem(struct item *item, struct item **list, struct item **last);
void recalculatenumbers(void);
void calcoffsets();
int max_textw();
void cleanup();
char* cistrstr(const char *h, const char *n);
void insert(const char *str, ssize_t n);
size_t nextrune(int inc);
void resizeclient();
void get_width();
void get_mh();
void set_mode();
void handle();
