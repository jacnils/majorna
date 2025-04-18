#pragma once

#include <x11/x11_libs.hpp>
#include <options.hpp>
#include <vector>
#include <any>

#if X11
enum resource_type {
    String = 0,
    Integer = 1,
    Float = 2
};

struct ResourcePref {
    std::string name{};
    resource_type type{};
    std::any dst{};
};

static std::vector<ResourcePref> cols{
    { "font",			             String,  &font },
    { "color10",                     String,  &col_caretfg },
    { "color0",                      String,  &col_caretbg },
    { "color4",                      String,  &col_larrowfg },
    { "color4",                      String,  &col_rarrowfg },
    { "color10",                     String,  &col_itemnormfg },
    { "color10",                     String,  &col_itemnormfg2 },
    { "color10",                     String,  &col_itemnormprifg },
    { "color10",                     String,  &col_inputfg },
    { "color10",                     String,  &col_pretextfg },
    { "color0",                      String,  &col_itemnormbg },
    { "color0",                      String,  &col_itemnormbg2 },
    { "color0",                      String,  &col_itemnormpribg },
    { "color0",                      String,  &col_menu },
    { "color0",                      String,  &col_larrowbg },
    { "color0",                      String,  &col_rarrowbg },
    { "color0",                      String,  &col_itemselfg },
    { "color0",                      String,  &col_itemmarkedfg },
    { "color0",                      String,  &col_itemselprifg },
    { "color0",                      String,  &col_inputbg },
    { "color0",                      String,  &col_pretextbg },
    { "color12",                     String,  &col_promptbg },
    { "color0",                      String,  &col_promptfg },
    { "color7",                      String,  &col_capsbg },
    { "color0",                      String,  &col_capsfg },
    { "color6",                      String,  &col_itemselbg },
    { "color6",                      String,  &col_itemmarkedbg },
    { "color6",                      String,  &col_itemselpribg },
    { "color6",                      String,  &col_border },
    { "color0",                      String,  &col_numfg },
    { "color5",                      String,  &col_numbg },
    { "color0",                      String,  &col_modefg },
    { "color11",                     String,  &col_modebg },
    { "color2",                      String,  &col_hlnormbg },
    { "color3",                      String,  &col_hlselbg },
    { "color0",                      String,  &col_hlnormfg },
    { "color0",                      String,  &col_hlselfg },
    { "color0",                      String,  &col_sgr0 },
    { "color1",                      String,  &col_sgr1 },
    { "color2",                      String,  &col_sgr2 },
    { "color3",                      String,  &col_sgr3 },
    { "color4",                      String,  &col_sgr4 },
    { "color5",                      String,  &col_sgr5 },
    { "color6",                      String,  &col_sgr6 },
    { "color7",                      String,  &col_sgr7 },
    { "color8",                      String,  &col_sgr8 },
    { "color9",                      String,  &col_sgr9 },
    { "color10",                     String,  &col_sgr10 },
    { "color11",                     String,  &col_sgr11 },
    { "color12",                     String,  &col_sgr12 },
    { "color13",                     String,  &col_sgr13 },
    { "color14",                     String,  &col_sgr14 },
    { "color15",                     String,  &col_sgr15 },
};

void load_xresources();
void resource_load(XrmDatabase db, const std::string& name, resource_type rtype, std::any& dst);
#endif
