#include <string>
#include <sockets.hpp>
#include <options.hpp>
#include <nlohmann/json.hpp>
#if X11
#include <x11/key.hpp>
#include <x11/mouse.hpp>
#endif
#if WAYLAND
#include <wl/wayland.hpp>
#endif
#include <config/config.hpp>
#define LIMHAMN_SOCKET_UDS_IMPL
#include <limhamn/socket/socket_uds.hpp>

using json = nlohmann::json;

template <typename T>
void get_template(json& j, const std::string& path, T& obj) {
    const auto split_path = [](const std::string& path) {
        std::vector<std::string> result;
        std::stringstream ss(path);
        std::string item;
        while (std::getline(ss, item, '/')) {
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        return result;
    };

    json* current = &j;
    for (const auto& key : split_path(path)) {
        if (!current->contains(key)) {
            return;
        }
        current = &(*current)[key];
    }
    if (!current->is_null()) {
        if (current->is_array() && current->size() == 1) {
            obj = current->at(0).get<T>();
            return;
        }
        try {
            obj = current->get<T>();
        } catch (...) {}
    }
};

void initialize_socket() {
    std::thread t([&]{
        if (std::filesystem::exists(socketfile)) {
            std::filesystem::remove(socketfile);
        }

        limhamn::socket::uds_server server(socketfile, handler, "\n", true);
    });

    t.detach();
}

void handle_keys(nlohmann::json& json) {
    if (!json.contains("keys") || !json.at("keys").is_array() || json.at("keys").empty()) {
        return;
    }

#if X11
    keys.clear();
#endif
#if WAYLAND
    wl_keys.clear();
#endif

    // iterate through the array
    for (const auto& it : json.at("keys")) {
#if X11
        Key key;
#endif
#if WAYLAND
        WlKey wl_key;
#endif

        if (it.contains("argument")) {
            if (it.at("argument").is_string()) {
#if X11
                key.arg.c = it.at("argument").get<std::string>();
#endif
#if WAYLAND
                wl_key.arg.c = it.at("argument").get<std::string>();
#endif
            } else if (it.at("argument").is_number_unsigned()) {
#if X11
                key.arg.i = key.arg.ui = it.at("argument").get<unsigned int>();
#endif
#if WAYLAND
                wl_key.arg.i = wl_key.arg.ui = it.at("argument").get<unsigned int>();
#endif
            } else if (it.at("argument").is_number_integer()) {
#if X11
                key.arg.i = it.at("argument").get<int>();
#endif
#if WAYLAND
                wl_key.arg.i = it.at("argument").get<int>();
#endif
            } else if (it.at("argument").is_number_float()) {
#if X11
                key.arg.f = it.at("argument").get<float>();
#endif
#if WAYLAND
                wl_key.arg.f = it.at("argument").get<float>();
#endif
            }
        }

        if (it.contains("function") && it.at("function").is_string()) {
            for (const auto& _it : fl) {
                if (_it.function == it.at("function").get<std::string>()) {
#if X11
                    key.func = _it.func;
#endif
#if WAYLAND
                    wl_key.func = _it.func;
#endif
                }
            }
        }

        if (it.contains("key") && it.at("key").is_string()) {
            for (const auto& _it : kl) {
                if (_it.key == it.at("key").get<std::string>()) {
#if X11
                    key.keysym = _it.keysym;
#endif
#if WAYLAND
                    wl_key.keysym = _it.keysym;
#endif
                }
            }
        }

        if (it.contains("mode") && it.at("mode").is_number_integer()) {
#if X11
            key.mode = it.at("mode").get<int>();
#endif
#if WAYLAND
            wl_key.mode = it.at("mode").get<int>();
#endif
        } else if (it.contains("mode") && it.at("mode").is_array() &&
                   it.at("mode").size() == 1 && it.at("mode").at(0).is_number_integer()) {
#if X11
            key.mode = it.at("mode").at(0).get<int>();
#endif
#if WAYLAND
            wl_key.mode = it.at("mode").at(0).get<int>();
#endif
        }

        if (it.contains("modifier") && it.at("modifier").is_string()) {
#if X11
            for (const auto& _it : ml) {
                if (_it.mod == it.at("modifier").get<std::string>()) {
                    key.mod = _it.modifier;
                }
            }
#endif
#if WAYLAND
            for (const auto& _it : wml) {
                if (_it.mod == it.at("modifier").get<std::string>()) {
                    wl_key.mod = _it.modifier;
                }
            }
#endif
        }

#if X11
        keys.push_back(key);
#endif
#if WAYLAND
        wl_keys.push_back(wl_key);
#endif
    }
}

void handle_mouse(nlohmann::json& json) {
    if (!json.contains("mouse") || !json.at("mouse").is_array() || json.at("mouse").empty()) {
        return;
    }

#if X11
    buttons.clear();
#endif
#if WAYLAND
    wl_buttons.clear();
#endif

    // iterate through the array
    for (const auto& it : json.at("mouse")) {
#if X11
        Mouse mouse{};
#endif
#if WAYLAND
        WlMouse wl_mouse{};
#endif

        if (it.contains("argument")) {
            if (it.at("argument").is_string()) {
#if X11
                mouse.arg.c = it.at("argument").get<std::string>();
#endif
#if WAYLAND
                wl_mouse.arg.c = it.at("argument").get<std::string>();
#endif
            } else if (it.at("argument").is_number_unsigned()) {
#if X11
                mouse.arg.i = mouse.arg.ui = it.at("argument").get<unsigned int>();
#endif
#if WAYLAND
                wl_mouse.arg.i = wl_mouse.arg.ui = it.at("argument").get<unsigned int>();
#endif
            } else if (it.at("argument").is_number_integer()) {
#if X11
                mouse.arg.i = it.at("argument").get<int>();
#endif
#if WAYLAND
                wl_mouse.arg.i = it.at("argument").get<int>();
#endif
            } else if (it.at("argument").is_number_float()) {
#if X11
                mouse.arg.f = it.at("argument").get<float>();
#endif
#if WAYLAND
                wl_mouse.arg.f = it.at("argument").get<float>();
#endif
            }
        }

        if (it.contains("function") && it.at("function").is_string()) {
            for (const auto& _it : fl) {
                if (_it.function == it.at("function").get<std::string>()) {
#if X11
                    mouse.func = _it.func;
#endif
#if WAYLAND
                    wl_mouse.func = _it.func;
#endif
                }
            }
        } else {
            throw std::runtime_error{"Invalid defined mouse binding in config file"};
        }

        if (it.contains("button") && it.at("button").is_number_integer()) {
#if X11
            if (x_button_list.size() >= it.at("button").get<int>()) {
                mouse.button = x_button_list.at(it.at("button").get<int>());
            }
#endif
#if WAYLAND
            if (wl_button_list.size() >= it.at("button").get<int>()) {
                wl_mouse.button = wl_button_list.at(it.at("button").get<int>());
            }
#endif
        } else if (it.contains("button") && it.at("button").is_array() &&
                   it.at("button").size() == 1 && it.at("button").at(0).is_number_integer())
        {
#if X11
            if (x_button_list.size() >= it.at("button").at(0).get<int>()) {
                mouse.button = x_button_list.at(it.at("button").at(0).get<int>());
            }
#endif
#if WAYLAND
            if (wl_button_list.size() >= it.at("button").at(0).get<int>()) {
                wl_mouse.button = wl_button_list.at(it.at("button").at(0).get<int>());
            }
#endif
        } else {
	        throw std::runtime_error{"Invalid defined mouse binding in config file"};
	    }

        if (it.contains("click") && it.at("click").is_number_integer()) {
#if X11
            mouse.click = static_cast<ClickType>(it.at("click").get<int>());
#endif
#if WAYLAND
            wl_mouse.click = static_cast<ClickType>(it.at("click").get<int>());
#endif
        } else if (it.contains("click") && it.at("click").is_array() &&
               it.at("click").size() == 1 && it.at("click").at(0).is_number_integer())
        {
#if X11
            mouse.click = static_cast<ClickType>(it.at("click").at(0).get<int>());
#endif
#if WAYLAND
            wl_mouse.click = static_cast<ClickType>(it.at("click").at(0).get<int>());
#endif
        } else {
            throw std::runtime_error{"Invalid defined mouse binding in config file"};
        }

#if X11
        buttons.push_back(mouse);
#endif
#if WAYLAND
        wl_buttons.push_back(wl_mouse);
#endif
    }
}

void load_config(nlohmann::json& json) {
    get_template<int>(json, "/appearance/caret/dimensions/height", caretheight);
    get_template<int>(json, "/appearance/caret/dimensions/width", caretwidth);
    get_template<int>(json, "/appearance/caret/dimensions/padding", caretpadding);

    get_template<std::string>(json, "/appearance/color/caps_lock_indicator/background", col_capsbg);
    get_template<std::string>(json, "/appearance/color/caps_lock_indicator/foreground", col_capsfg);

    get_template<std::string>(json, "/appearance/color/caret/background", col_caretbg);
    get_template<std::string>(json, "/appearance/color/caret/foreground", col_caretfg);

    get_template<std::string>(json, "/appearance/color/general/border", col_border);
    get_template<std::string>(json, "/appearance/color/general/menu", col_menu);

    get_template<std::string>(json, "/appearance/color/highlighting/normal_background", col_hlnormbg);
    get_template<std::string>(json, "/appearance/color/highlighting/normal_foreground", col_hlnormfg);
    get_template<std::string>(json, "/appearance/color/highlighting/selected_background", col_hlselbg);
    get_template<std::string>(json, "/appearance/color/highlighting/selected_foreground", col_hlselfg);

    get_template<std::string>(json, "/appearance/color/input/background", col_inputbg);
    get_template<std::string>(json, "/appearance/color/input/foreground", col_inputfg);

    get_template<std::string>(json, "/appearance/color/item/marked_background", col_itemmarkedbg);
    get_template<std::string>(json, "/appearance/color/item/marked_foreground", col_itemmarkedfg);
    get_template<std::string>(json, "/appearance/color/item/normal_background", col_itemnormbg);
    get_template<std::string>(json, "/appearance/color/item/normal_background_priority", col_itemnormpribg);
    get_template<std::string>(json, "/appearance/color/item/normal_background_second", col_itemnormbg);
    get_template<std::string>(json, "/appearance/color/item/normal_foreground", col_itemnormfg);
    get_template<std::string>(json, "/appearance/color/item/normal_foreground_priority", col_itemnormprifg);
    get_template<std::string>(json, "/appearance/color/item/normal_foreground_second", col_itemnormfg2);
    get_template<std::string>(json, "/appearance/color/item/selected_background", col_itemselbg);
    get_template<std::string>(json, "/appearance/color/item/selected_background_priority", col_itemselpribg);
    get_template<std::string>(json, "/appearance/color/item/selected_background_second", col_itemselbg2);
    get_template<std::string>(json, "/appearance/color/item/selected_foreground", col_itemselfg);
    get_template<std::string>(json, "/appearance/color/item/selected_foreground_priority", col_itemselprifg);
    get_template<std::string>(json, "/appearance/color/item/selected_foreground_second", col_itemselfg2);

    get_template<std::string>(json, "/appearance/color/left_arrow/background", col_larrowbg);
    get_template<std::string>(json, "/appearance/color/left_arrow/foreground", col_larrowfg);

    get_template<std::string>(json, "/appearance/color/match_counter/background", col_numbg);
    get_template<std::string>(json, "/appearance/color/match_counter/foreground", col_numfg);

    get_template<std::string>(json, "/appearance/color/mode_indicator/background", col_modebg);
    get_template<std::string>(json, "/appearance/color/mode_indicator/foreground", col_modefg);

    get_template<std::string>(json, "/appearance/color/pretext/background", col_pretextbg);
    get_template<std::string>(json, "/appearance/color/pretext/foreground", col_pretextfg);

    get_template<std::string>(json, "/appearance/color/prompt/background", col_promptbg);
    get_template<std::string>(json, "/appearance/color/prompt/foreground", col_promptfg);

    get_template<std::string>(json, "/appearance/color/right_arrow/background", col_rarrowbg);
    get_template<std::string>(json, "/appearance/color/right_arrow/foreground", col_rarrowfg);

    std::vector<std::string> sgr_colors{};
    get_template<std::vector<std::string>>(json, "/appearance/color/sgr", sgr_colors);

    for (int i{0}; i < 16; i++) {
        if (sgr_colors.size() <= i) break;
        if (sgr_colors[i].empty()) continue;

        if (i == 0) col_sgr0 = sgr_colors.at(i);
        if (i == 1) col_sgr1 = sgr_colors.at(i);
        if (i == 2) col_sgr2 = sgr_colors.at(i);
        if (i == 3) col_sgr3 = sgr_colors.at(i);
        if (i == 4) col_sgr4 = sgr_colors.at(i);
        if (i == 5) col_sgr5 = sgr_colors.at(i);
        if (i == 6) col_sgr6 = sgr_colors.at(i);
        if (i == 7) col_sgr7 = sgr_colors.at(i);
        if (i == 8) col_sgr8 = sgr_colors.at(i);
        if (i == 9) col_sgr9 = sgr_colors.at(i);
        if (i == 10) col_sgr10 = sgr_colors.at(i);
        if (i == 11) col_sgr11 = sgr_colors.at(i);
        if (i == 12) col_sgr12 = sgr_colors.at(i);
        if (i == 13) col_sgr13 = sgr_colors.at(i);
        if (i == 14) col_sgr14 = sgr_colors.at(i);
        if (i == 15) col_sgr15 = sgr_colors.at(i);
    }

    get_template<bool>(json, "/appearance/color_items",  coloritems);
    get_template<bool>(json, "/appearance/enable_sgr_sequences", sgr);
    get_template<double>(json, "/appearance/input/width", inputwidth);
    get_template<int>(json, "/appearance/line/columns", columns);
    get_template<int>(json, "/appearance/line/height", lineheight);
    get_template<bool>(json, "/appearance/line/indent_items_to_prompt", indentitems);
    get_template<int>(json, "/appearance/line/item_position", itemposition);
    get_template<int>(json, "/appearance/line/lines", lines);
    get_template<int>(json, "/appearance/line/minimum_lines", minlines);

    get_template<bool>(json, "/appearance/powerline/enable/caps_lock_indicator", powerlinecaps);
    get_template<bool>(json, "/appearance/powerline/enable/item", powerlineitems);
    get_template<bool>(json, "/appearance/powerline/enable/match_counter", powerlinecount);
    get_template<bool>(json, "/appearance/powerline/enable/mode_indicator", powerlinemode);
    get_template<bool>(json, "/appearance/powerline/enable/prompt", powerlineprompt);

    get_template<int>(json, "/appearance/powerline/style/caps_lock_indicator_style", capspwlstyle);
    get_template<int>(json, "/appearance/powerline/style/item_style", itempwlstyle);
    get_template<int>(json, "/appearance/powerline/style/match_counter_style", matchcountpwlstyle);
    get_template<int>(json, "/appearance/powerline/style/mode_indicator_style", modepwlstyle);
    get_template<int>(json, "/appearance/powerline/style/prompt_style", promptpwlstyle);

    get_template<bool>(json, "/appearance/hide/caps_lock_indicator", hidecaps);
    get_template<bool>(json, "/appearance/hide/caret", hidecaret);
    get_template<bool>(json, "/appearance/hide/highlighting", hidehighlight);
    get_template<bool>(json, "/appearance/hide/images", hideimage);
    get_template<bool>(json, "/appearance/hide/input", hideinput);
    get_template<bool>(json, "/appearance/hide/items", hideitem);
    get_template<bool>(json, "/appearance/hide/left_arrow", hidelarrow);
    get_template<bool>(json, "/appearance/hide/match_counter", hidematchcount);
    get_template<bool>(json, "/appearance/hide/mode_indicator", hidemode);
    get_template<bool>(json, "/appearance/hide/prompt", hideprompt);
    get_template<bool>(json, "/appearance/hide/pretext", hidepretext);
    get_template<bool>(json, "/appearance/hide/powerline", hidepowerline);
    get_template<bool>(json, "/appearance/hide/right_arrow", hiderarrow);

    get_template<int>(json, "/appearance/transparency/caps_lock_indicator/background", alpha_capsbg);
    get_template<int>(json, "/appearance/transparency/caps_lock_indicator/foreground", alpha_capsfg);

    get_template<int>(json, "/appearance/transparency/caret/background", alpha_caretbg);
    get_template<int>(json, "/appearance/transparency/caret/foreground", alpha_caretfg);

    get_template<int>(json, "/appearance/transparency/general/border", alpha_border);
    get_template<int>(json, "/appearance/transparency/general/menu", alpha_menu);

    get_template<int>(json, "/appearance/transparency/highlighting/normal_background", alpha_hlnormbg);
    get_template<int>(json, "/appearance/transparency/highlighting/normal_foreground", alpha_hlnormfg);
    get_template<int>(json, "/appearance/transparency/highlighting/selected_background", alpha_hlselbg);
    get_template<int>(json, "/appearance/transparency/highlighting/selected_foreground", alpha_hlselfg);

    get_template<int>(json, "/appearance/transparency/input/background", alpha_inputbg);
    get_template<int>(json, "/appearance/transparency/input/foreground", alpha_inputfg);

    get_template<int>(json, "/appearance/transparency/item/marked_background", alpha_itemmarkedbg);
    get_template<int>(json, "/appearance/transparency/item/marked_foreground", alpha_itemmarkedfg);
    get_template<int>(json, "/appearance/transparency/item/normal_background", alpha_itemnormbg);
    get_template<int>(json, "/appearance/transparency/item/normal_background_priority", alpha_itemnormpribg);
    get_template<int>(json, "/appearance/transparency/item/normal_background_second", alpha_itemnormbg);
    get_template<int>(json, "/appearance/transparency/item/normal_foreground", alpha_itemnormfg);
    get_template<int>(json, "/appearance/transparency/item/normal_foreground_priority", alpha_itemnormprifg);
    get_template<int>(json, "/appearance/transparency/item/normal_foreground_second", alpha_itemnormfg2);
    get_template<int>(json, "/appearance/transparency/item/selected_background", alpha_itemselbg);
    get_template<int>(json, "/appearance/transparency/item/selected_background_priority", alpha_itemselpribg);
    get_template<int>(json, "/appearance/transparency/item/selected_background_second", alpha_itemselbg2);
    get_template<int>(json, "/appearance/transparency/item/selected_foreground", alpha_itemselfg);
    get_template<int>(json, "/appearance/transparency/item/selected_foreground_priority", alpha_itemselprifg);
    get_template<int>(json, "/appearance/transparency/item/selected_foreground_second", alpha_itemselfg2);

    get_template<int>(json, "/appearance/transparency/left_arrow/background", alpha_larrowbg);
    get_template<int>(json, "/appearance/transparency/left_arrow/foreground", alpha_larrowfg);

    get_template<int>(json, "/appearance/transparency/match_counter/background", alpha_numbg);
    get_template<int>(json, "/appearance/transparency/match_counter/foreground", alpha_numfg);

    get_template<int>(json, "/appearance/transparency/mode_indicator/background", alpha_modebg);
    get_template<int>(json, "/appearance/transparency/mode_indicator/foreground", alpha_modefg);

    get_template<int>(json, "/appearance/transparency/pretext/background", alpha_pretextbg);
    get_template<int>(json, "/appearance/transparency/pretext/foreground", alpha_pretextfg);

    get_template<int>(json, "/appearance/transparency/prompt/background", alpha_promptbg);
    get_template<int>(json, "/appearance/transparency/prompt/foreground", alpha_promptfg);

    get_template<int>(json, "/appearance/transparency/right_arrow/background", alpha_rarrowbg);
    get_template<int>(json, "/appearance/transparency/right_arrow/foreground", alpha_rarrowfg);

    get_template<int>(json, "/behavior/display/preselected", preselected);
    get_template<bool>(json, "/behavior/display/sort", sortmatches);

    get_template<bool>(json, "/behavior/matching/case_sensitive", casesensitive);
    get_template<bool>(json, "/behavior/matching/fuzzy_matching", fuzzy);
    get_template<bool>(json, "/behavior/matching/regex_matching", regex);

    get_template<std::string>(json, "/behavior/miscellaneous/delimiters", worddelimiters);
    get_template<bool>(json, "/behavior/miscellaneous/fast", fast);
    get_template<bool>(json, "/behavior/miscellaneous/force_insert_mode", forceinsertmode);
    get_template<std::string>(json, "/behavior/miscellaneous/list_file", listfile);
    get_template<int>(json, "/behavior/miscellaneous/scroll_distance", scrolldistance);
    get_template<int>(json, "/behavior/miscellaneous/start_mode", mode);

    get_template<bool>(json, "/behavior/output/incremental", incremental);
    get_template<bool>(json, "/behavior/output/print_index", printindex);

    get_template<bool>(json, "/behavior/pango/caps_lock_indicator", pango_caps);
    get_template<bool>(json, "/behavior/pango/input", pango_input);
    get_template<bool>(json, "/behavior/pango/item", pango_item);
    get_template<bool>(json, "/behavior/pango/left_arrow", pango_leftarrow);
    get_template<bool>(json, "/behavior/pango/match_counter", pango_numbers);
    get_template<bool>(json, "/behavior/pango/mode_indicator", pango_mode);
    get_template<bool>(json, "/behavior/pango/password", pango_password);
    get_template<bool>(json, "/behavior/pango/pretext", pango_pretext);
    get_template<bool>(json, "/behavior/pango/prompt", pango_prompt);
    get_template<bool>(json, "/behavior/pango/right_arrow", pango_rightarrow);

    get_template<bool>(json, "/behavior/permissions/mark", mark);
    get_template<bool>(json, "/behavior/permissions/password", passwd);
    get_template<bool>(json, "/behavior/permissions/typing", type);

    get_template<std::string>(json, "/filesystem/paths/fifo", fifofile);
    get_template<std::string>(json, "/filesystem/paths/image_cache_directory", cachedir);
    get_template<std::string>(json, "/filesystem/paths/screenshot", screenshotfile);

    get_template<int>(json, "/history/max_number_of_entries", maxhist);
    get_template<bool>(json, "/history/save_duplicates", histdup);

    get_template<bool>(json, "/image/cache", generatecache);
    get_template<int>(json, "/image/dimensions/width", imagewidth);
    get_template<int>(json, "/image/dimensions/height", imageheight);
    get_template<int>(json, "/image/dimensions/gaps", imagegaps);
    get_template<int>(json, "/image/max_size_to_cache", maxcache);
    get_template<int>(json, "/image/position", imageposition);
    get_template<bool>(json, "/image/resize_to_fit", imageresize);
    get_template<int>(json, "/image/type", imagetype);

    get_template<std::string>(json, "/text/caps_lock_off", capslockofftext);
    get_template<std::string>(json, "/text/caps_lock_on", capslockontext);
    get_template<std::string>(json, "/text/font", font);
    get_template<std::string>(json, "/text/input", input);
    get_template<std::string>(json, "/text/insert_mode", instext);
    get_template<std::string>(json, "/text/left_arrow", leftarrow);
    get_template<int>(json, "/text/normal_item_padding", normitempadding);
    get_template<std::string>(json, "/text/normal_mode", normtext);
    get_template<int>(json, "/text/padding", textpadding);
    get_template<std::string>(json, "/text/password", password);
    get_template<std::string>(json, "/text/pretext", pretext);
    get_template<int>(json, "/text/priority_item_padding", priitempadding);
    get_template<std::string>(json, "/text/prompt", prompt);
    get_template<std::string>(json, "/text/regex_mode", regextext);
    get_template<std::string>(json, "/text/right_arrow", rightarrow);
    get_template<int>(json, "/text/selected_item_padding", selitempadding);

    get_template<int>(json, "/window/border_size", borderwidth);
    get_template<int>(json, "/window/center_width", centerwidth);
    get_template<int>(json, "/window/dimensions/x", xpos);
    get_template<int>(json, "/window/dimensions/y", ypos);
    get_template<int>(json, "/window/dimensions/width", menuwidth);
    get_template<bool>(json, "/window/enable_transparency", alpha);
    get_template<bool>(json, "/window/grab_input", grabkeyboard);
    get_template<int>(json, "/window/margin_horizontal", menumarginh);
    get_template<int>(json, "/window/margin_vertical", menumarginv);
    get_template<int>(json, "/window/padding_horizontal", menupaddingh);
    get_template<int>(json, "/window/padding_vertical", menupaddingv);
    get_template<int>(json, "/window/monitor", mon);
    get_template<int>(json, "/window/position", menuposition);
    get_template<int>(json, "/window/protocol", protocol);

    get_template<std::string>(json, "/x11/class_name", window_class);
    get_template<bool>(json, "/x11/dock_type", dockproperty);
    get_template<bool>(json, "/x11/managed", managed);
    get_template<bool>(json, "/x11/xresources", xresources);

    handle_keys(json);
    handle_mouse(json);
}

std::string handler(const std::string& input) {
    nlohmann::json input_json{};

    try {
        input_json = nlohmann::json::parse(input);
    } catch (const std::exception&) {
        nlohmann::json return_json;
        return_json["error_str"] = "Invalid JSON.";
        return_json["error_type"] = "MAJORNA_INVALID_JSON";
        return return_json.dump();
    }

    nlohmann::json return_json{};

    return_json["actions_performed"] = nlohmann::json::array();

    if (input_json.contains("action") && input_json["action"].is_string()) {
        const std::string& action = input_json["action"].get<std::string>();

        if (action == "load_config") {
            nlohmann::json return_json{};

            if (input_json.contains("config_json") && input_json["config_json"].is_string()) {
                try {
                    nlohmann::json config_json = nlohmann::json::parse(input_json.at("config_json").get<std::string>());
                    load_config(config_json);
                } catch (const std::exception&) {
                    return_json["error_str"] = "Invalid JSON.";
                    return_json["error_type"] = "MAJORNA_INVALID_JSON";
                    return return_json.dump();
                }
            } else {
                return_json["error_str"] = "Invalid JSON.";
                return_json["error_type"] = "MAJORNA_INVALID_JSON";
                return return_json.dump();
            }

            return_json["actions_performed"] = nlohmann::json::array();
            return_json["actions_performed"].push_back("load_config");

            return return_json.dump();
        } else if (action == "draw_rect") {
            while (!ctx.initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            RectDrawProperty p{};
            if (input_json.contains("rect") && input_json["rect"].is_object()) {
                const auto& rect = input_json["rect"];
                if (rect.contains("x") && rect["x"].is_number_integer()) {
                    p.pos.x = rect["x"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
                if (rect.contains("y") && rect["y"].is_number_integer()) {
                    p.pos.y = rect["y"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
                if (rect.contains("width") && rect["width"].is_number_integer()) {
                    p.pos.w = rect["width"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
                if (rect.contains("height") && rect["height"].is_number_integer()) {
                    p.pos.h = rect["height"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
            }
            if (input_json.contains("key") && input_json["key"].is_string()) {
                p.key = input_json["key"].get<std::string>();
            } else {
                throw std::runtime_error{"Invalid defined key in config file"};
            }
            if (input_json.contains("filled") && input_json["filled"].is_boolean()) {
                p.props.filled = input_json["filled"].get<bool>();
            } else {
                throw std::runtime_error{"Invalid defined filled in config file"};
            }
            if (input_json.contains("invert") && input_json["invert"].is_boolean()) {
                p.props.invert = input_json["invert"].get<bool>();
            } else {
                throw std::runtime_error{"Invalid defined invert in config file"};
            }
            if (input_json.contains("color") && input_json["color"].is_string()) {
                p.props.background = input_json["color"].get<std::string>();
            } else {
                throw std::runtime_error{"Invalid defined color in config file"};
            }
            if (input_json.contains("alpha") && input_json["alpha"].is_number_integer()) {
                p.props.background_alpha = input_json["alpha"].get<int>();
            } else {
                throw std::runtime_error{"Invalid defined alpha in config file"};
            }

            if (input_json.contains("no_print_standard") && input_json["no_print_standard"].is_boolean()) {
                no_print_standard = input_json["no_print_standard"].get<bool>();
            }

            rect_prop.push_back(p);

            return_json["original_x"] = p.pos.x;
            return_json["original_y"] = p.pos.y;
            return_json["x"] = p.pos.x + p.pos.w;
            return_json["y"] = p.pos.y + p.pos.h;
            return_json["width"] = p.pos.w;
            return_json["height"] = p.pos.h;
            return_json["key"] = p.key;
            return_json["actions_performed"].push_back("draw_rect");
            return return_json.dump();
        } else if (action == "rm_rect") {
            while (!ctx.initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (input_json.contains("key") && input_json["key"].is_string()) {
                const std::string& key = input_json["key"].get<std::string>();
                auto it = std::remove_if(rect_prop.begin(), rect_prop.end(), [&key](const RectDrawProperty& p) {
                    return p.key == key;
                });
                rect_prop.erase(it, rect_prop.end());
            } else {
                throw std::runtime_error{"Invalid defined key in config file"};
            }

            if (input_json.contains("no_print_standard") && input_json["no_print_standard"].is_boolean()) {
                no_print_standard = input_json["no_print_standard"].get<bool>();
            }

            return_json["actions_performed"].push_back("rm_rect");
            return return_json.dump();
        } else if (action == "clear_rect") {
            while (!ctx.initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (input_json.contains("no_print_standard") && input_json["no_print_standard"].is_boolean()) {
                no_print_standard = input_json["no_print_standard"].get<bool>();
            }

            rect_prop.clear();

            return_json["actions_performed"].push_back("clear_rect");
            return return_json.dump();
        } else if (action == "draw_text") {
            while (!ctx.initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            TextDrawProperty p{};
            if (input_json.contains("rect") && input_json["rect"].is_object()) {
                const auto& rect = input_json["rect"];
                if (rect.contains("x") && rect["x"].is_number_integer()) {
                    p.pos.x = rect["x"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
                if (rect.contains("y") && rect["y"].is_number_integer()) {
                    p.pos.y = rect["y"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
                if (rect.contains("width") && rect["width"].is_number_integer()) {
                    p.pos.w = rect["width"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
                if (rect.contains("height") && rect["height"].is_number_integer()) {
                    p.pos.h = rect["height"].get<int>();
                } else {
                    throw std::runtime_error{"Invalid defined rectangle in config file"};
                }
            }
            if (input_json.contains("key") && input_json["key"].is_string()) {
                p.key = input_json["key"].get<std::string>();
            } else {
                throw std::runtime_error{"Invalid defined key in config file"};
            }
            if (input_json.contains("filled") && input_json["filled"].is_boolean()) {
                p.props.filled = input_json["filled"].get<bool>();
            } else {
                throw std::runtime_error{"Invalid defined filled in config file"};
            }
            if (input_json.contains("invert") && input_json["invert"].is_boolean()) {
                p.props.invert = input_json["invert"].get<bool>();
            } else {
                throw std::runtime_error{"Invalid defined invert in config file"};
            }
            if (input_json.contains("foreground_color") && input_json["foreground_color"].is_string()) {
                p.props.foreground = input_json["foreground_color"].get<std::string>();
            } else {
                throw std::runtime_error{"Invalid defined color in config file"};
            }
            if (input_json.contains("foreground_alpha") && input_json["foreground_alpha"].is_number_integer()) {
                p.props.foreground_alpha = input_json["foreground_alpha"].get<int>();
            } else {
                throw std::runtime_error{"Invalid defined alpha in config file"};
            }
            if (input_json.contains("background_color") && input_json["background_color"].is_string()) {
                p.props.background = input_json["background_color"].get<std::string>();
            } else {
                throw std::runtime_error{"Invalid defined color in config file"};
            }
            if (input_json.contains("background_alpha") && input_json["background_alpha"].is_number_integer()) {
                p.props.background_alpha = input_json["background_alpha"].get<int>();
            } else {
                throw std::runtime_error{"Invalid defined alpha in config file"};
            }

            if (input_json.contains("no_print_standard") && input_json["no_print_standard"].is_boolean()) {
                no_print_standard = input_json["no_print_standard"].get<bool>();
            }

            if (input_json.contains("text") && input_json["text"].is_string()) {
                p.input_text = input_json["text"].get<std::string>();
            } else {
                throw std::runtime_error{"Invalid defined text in config file"};
            }

            if (input_json.contains("padding") && input_json["padding"].is_number_integer()) {
                p.padding = input_json["padding"].get<int>();
            }

            if (input_json.contains("markup") && input_json["markup"].is_boolean()) {
                p.markup = input_json["markup"].get<bool>();
            }

            int width = draw.get_text_width(p.input_text);

            return_json["original_x"] = p.pos.x;
            return_json["original_y"] = p.pos.y;
            return_json["x"] = p.pos.x + width;
            return_json["y"] = p.pos.y;
            return_json["width"] = width;
            return_json["height"] = p.pos.h;
            return_json["key"] = p.key;
            return_json["actions_performed"].push_back("draw_text");

            text_prop.push_back(p);

            return return_json.dump();

        } else if (action == "rm_text") {
            while (!ctx.initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (input_json.contains("key") && input_json["key"].is_string()) {
                const std::string& key = input_json["key"].get<std::string>();
                auto it = std::remove_if(text_prop.begin(), text_prop.end(), [&key](const TextDrawProperty& p) {
                    return p.key == key;
                });
                text_prop.erase(it, text_prop.end());

                return_json["actions_performed"].push_back("rm_text");
                return return_json.dump();
            } else {
                throw std::runtime_error{"Invalid defined key in config file"};
            }
        } else if (action == "draw_menu") {
            while (!ctx.initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            if (input_json.contains("no_print_standard") && input_json["no_print_standard"].is_boolean()) {
                no_print_standard = input_json["no_print_standard"].get<bool>();
            }

            draw_menu();

            return_json["actions_performed"].push_back("draw_menu");
            return return_json.dump();
        }
    }

    std::vector<std::string> functions_called{};
    if (input_json.contains("calls") && input_json["calls"].is_array()) {
        for (const auto& call : input_json["calls"]) {
            if (!call.is_object()) continue;
            if (!call.contains("function") || !call["function"].is_string()) continue;

            while (!ctx.initialized) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            const std::string& fn = call["function"].get<std::string>();

            const auto it = arg_map.find(fn);
            if (it == arg_map.end()) {
                const auto v_it = void_map.find(fn);
                if (v_it == void_map.end()) {
                    return_json = {};
                    return_json["error_str"] = "Function '" + fn + "' not found.";
                    return_json["error_type"] = "MAJORNA_FUNCTION_NOT_FOUND";
                    return return_json.dump();
                }

                if (call.contains("arg")) {
                    return_json = {};
                    return_json["error_str"] = "Function does not accept arguments.";
                    return_json["error_type"] = "MAJORNA_INVALID_ARGUMENT";
                    return return_json.dump();
                }

                if (v_it->second) {
                    v_it->second();
                    functions_called.push_back(fn);
                } else {
                    return_json = {};
                    return_json["error_str"] = "Function is not valid. This is probably an error out of your control. Function call: " + fn;
                    return_json["error_type"] = "MAJORNA_INVALID_FUNCTION";
                    return return_json.dump();
                }
            } else {
                Arg arg{};
                if (call.contains("arg")) {
                    const auto& arg_val = call["arg"];

                    if (arg_val.is_string()) {
                        arg.c = arg_val.get<std::string>();
                    } else if (arg_val.is_number_unsigned()) {
                        arg.ui = arg.i = arg_val.get<unsigned int>();
                    } else if (arg_val.is_number_integer()) {
                        arg.i = arg_val.get<int>();
                    } else if (arg_val.is_number_float()) {
                        arg.f = arg_val.get<float>();
                    }
                }

                if (it->second) {
                    it->second(arg);
                    functions_called.push_back(fn);
                } else {
                    return_json = {};
                    return_json["error_str"] = "Function is not valid. This is probably an error out of your control. Function call: " + fn;
                    return_json["error_type"] = "MAJORNA_INVALID_FUNCTION";
                    return return_json.dump();
                }
            }
        }

        return_json["actions_performed"].push_back("calls");
        return_json["functions_called"] = functions_called;
    }

    return return_json.dump();
}
