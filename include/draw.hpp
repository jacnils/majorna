/* See LICENSE file for copyright and license details. */

#pragma once

#include <limhamn/primitive/primitive.hpp>

struct RectDrawProperty {
    limhamn::primitive::draw_position pos{};
    limhamn::primitive::draw_properties props{};
    std::string key{};
    int new_x{};
};

struct TextDrawProperty {
    limhamn::primitive::draw_position pos{};
    int padding{};
    std::string input_text{};
    bool markup{};
    limhamn::primitive::draw_properties props{};
    std::string key{};
    int new_x{};
};

inline std::vector<RectDrawProperty> rect_prop;
inline std::vector<TextDrawProperty> text_prop;
inline bool no_print_standard{false};

void drawhighlights(struct item *item, int x, int y, int w, int p, char *itemtext);
char* get_text_n_sgr(struct item *item);
int drawitemtext(struct item *item, int x, int y, int w);
int drawitem(int x, int y, int w);
int drawprompt(int x, int y, int w);
int drawinput(int x, int y, int w);
int drawlarrow(int x, int y, int w);
int drawrarrow(int x, int y, int w);
int drawnumber(int x, int y, int w);
int drawmode(int x, int y, int w);
int draw_caps_indicator(int x, int y, int w);
void draw_menu();
void draw_menu_layer();
#if IMAGE
int draw_icon(struct item *item, int x, int y, int w, int h);
#endif
