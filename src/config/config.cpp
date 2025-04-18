/* See LICENSE file for copyright and license details. */
#if CONFIG
#include <libconfig.h>
#include <config/config.hpp>
#include <options.hpp>
#include <cstring>
#include <cstdlib>
#include <macros.hpp>
#include <x11/key.hpp>
#include <wl/wayland.hpp>
#include <x11/mouse.hpp>
#include <majorna.hpp>
#include <schemes.hpp>
#include <filesystem>
#include <iostream>

int bind_init() {
    std::string bnfile{};
    const char *dest;
    int ret = 0;

    // get path for configuration file
    if (bindsfile.empty()) {
        const char* xdg_conf = getenv("XDG_CONFIG_HOME");
        if (!xdg_conf) {
            // ~/.config/majorna/binds.conf
            const char* home = getenv("HOME");
            if (!home) {
                die("majorna: HOME environment variable is not set");
            }
            bnfile = std::string(home) + "/.config/majorna/binds.conf";
        } else {
            // XDG_CONFIG_HOME is set, so let's use that instead
            bnfile = std::string(xdg_conf) + "/majorna/binds.conf";
        }
    } else { // custom config path
        bnfile = bindsfile;
    }

    if (!std::filesystem::is_regular_file(bnfile)) {
        return 0;
    }

    // init config
    config_t bind;

    config_init(&bind);

    // attempt to read config file to cfg
    if (!config_read_file(&bind, bnfile.c_str())) {
        // invalid configuration, but let's try to read it anyway
        std::cerr << "majorna: Invalid keys file." << std::endl;
    }

    // load options bind.keys
    config_setting_t *key_bind = config_lookup(&bind, "bind.keys");
    if (key_bind != nullptr && loadbinds) {
        int nmode = 0;

        ret = 1;

        for (unsigned int i = 0; i < config_setting_length(key_bind); ++i) {
            config_setting_t *conf = config_setting_get_elem(key_bind, i);

            // look up
            config_setting_lookup_string(conf, "modifier", &dest);

#if X11
            Key key{};
#endif
#if WAYLAND
            WlKey wl_key{};
#endif

#if X11
            for (const auto& it : ml) {
                if (it.mod == dest) {
                    key.mod = it.modifier;
                }
            }
#endif

#if WAYLAND
            for (const auto& it : wml) {
                if (it.mod == dest) {
                    wl_key.mod = it.modifier;
                }
            }
#endif

            if (config_setting_lookup_int(conf, "mode", &nmode)) {
                if (nmode < 0) nmode = -1;
                if (nmode > 1) nmode = 1;

#if X11
                key.mode = nmode;
#endif
#if WAYLAND
                wl_key.mode = nmode;
#endif
            }

            config_setting_lookup_string(conf, "key", &dest);

#if X11
            for (const auto& it : kl) {
                if (it.key == dest) {
                    key.keysym = it.keysym;
                }
            }
#endif

#if WAYLAND
            for (const auto& it : wkl) {
                if (it.key == dest) {
                    wl_key.keysym = it.keysym;
                }
            }
#endif

            config_setting_lookup_string(conf, "function", &dest);

            for (const auto& it : fl) {
                if (it.function == dest) {
#if X11
                    key.func = it.func;
#endif
#if WAYLAND
                    wl_key.func = it.func;
#endif
                }
            }

            config_setting_lookup_string(conf, "argument", &dest);

            // TODO: More types
            if (strdup(dest)) {
#if X11
                key.arg.i = std::stoi(dest);
#endif
#if WAYLAND
                wl_key.arg.i = std::stoi(dest);
#endif
            }

            config_setting_lookup_bool(conf, "forceinsertmode", reinterpret_cast<int*>(&forceinsertmode));
            config_setting_lookup_bool(conf, "ignoreglobalkeys", reinterpret_cast<int*>(&ctx.ignore_global_keys));

#if X11
            std::cerr << "global" << ctx.ignore_global_keys << "\n";
            ckeys.emplace_back(key);
#endif
#if WAYLAND
            wl_ckeys.emplace_back(wl_key);
#endif
        }
    }

    // load options bind.mouse
    config_setting_t *mouse_bind = config_lookup(&bind, "bind.mouse");
    if (mouse_bind != nullptr && loadbinds) {
        ret = 1;

        for (unsigned int i = 0; i < config_setting_length(mouse_bind); ++i) {
            config_setting_t *conf = config_setting_get_elem(mouse_bind, i);

            config_setting_lookup_string(conf, "click", &dest);

#if X11
            Mouse mouse{};
#endif
#if WAYLAND
            WlMouse wl_mouse{};
#endif

            for (const auto& it : ctp) {
                if (it.tclick == dest) {
#if X11
                    mouse.click = it.click;
#endif
#if WAYLAND
                    mouse.click = it.click;
#endif
                }
            }

            config_setting_lookup_string(conf, "button", &dest);

#if X11
            for (const auto& it : btp) {
                if (it.click == dest) {
                    mouse.button = it.button;
                }
            }
#endif
#if WAYLAND
            for (const auto& it : w_btp) {
                if (it.click == dest) {
                    wl_mouse.button = it.button;
                }
            }
#endif

            config_setting_lookup_string(conf, "function", &dest);

            for (const auto& it : fl) {
                if (it.function == dest) {
#if X11
                    mouse.func = it.func;
#endif
#if WAYLAND
                    wl_mouse.func = it.func;
#endif
                }
            }

            config_setting_lookup_string(conf, "argument", &dest);

            if (strdup(dest)) {
#if X11
                mouse.arg.i = std::stoi(strdup(dest));
#endif
#if WAYLAND
                wl_mouse.arg.i = std::stoi(strdup(dest));
#endif
            }

            config_setting_lookup_bool(conf, "ignoreglobalmouse", reinterpret_cast<int*>(&ctx.ignore_global_mouse));
            config_setting_lookup_int(conf, "scrolldistance", &scrolldistance);

#if X11
            cbuttons.push_back(mouse);
#endif
#if WAYLAND
            wl_cbuttons.push_back(wl_mouse);
#endif
        }
    }

    std::cerr << ret << std::endl;

    // finally done
    config_destroy(&bind);

    return ret;
}


void conf_init() {
    if (!loadconfig) {
        return;
    }

    std::string cfgfile{};
    const char *dest;

    // get path for configuration file
    if (configfile.empty()) {
        const char* xdg_conf = getenv("XDG_CONFIG_HOME");
        if (!xdg_conf) {
            // ~/.config/majorna/majorna.conf
            const char* home = getenv("HOME");
            if (!home) {
                die("majorna: HOME environment variable is not set");
            }
            cfgfile = std::string(home) + "/.config/majorna/majorna.conf";
        } else {
            // XDG_CONFIG_HOME is set, so let's use that instead
            cfgfile = std::string(xdg_conf) + "/majorna/majorna.conf";
        }
    } else { // custom config path
        cfgfile = configfile;
    }

    if (!std::filesystem::is_regular_file(cfgfile)) {
        loadconfig = 0;
    }

    // init config
    config_t cfg;
    config_init(&cfg);

    // attempt to read config file to cfg
    if (loadconfig) {
        if (!config_read_file(&cfg, cfgfile.c_str())) {
            // invalid configuration, but let's try to read it anyway
            fprintf(stderr, "majorna: Invalid configuration.\n");
        }
    }

    // load options majorna.window
    config_setting_t *window_setting = config_lookup(&cfg, "majorna.window");
    if (window_setting != nullptr && loadconfig) {
        // look up window entries
        for (unsigned int i = 0; i < config_setting_length(window_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(window_setting, i);

            config_setting_lookup_int(conf, "position", &menuposition); // majorna.window.menuposition
            config_setting_lookup_int(conf, "paddingv", &menupaddingv); // majorna.window.paddingv
            config_setting_lookup_int(conf, "paddingh", &menupaddingh); // majorna.window.paddingh
            config_setting_lookup_int(conf, "padding-vertical", &menupaddingv); // majorna.window.padding-vertical
            config_setting_lookup_int(conf, "padding-horizontal", &menupaddingh); // majorna.window.padding-horizontal
            config_setting_lookup_int(conf, "margin-vertical", &menumarginv); // majorna.window.margin-vertical
            config_setting_lookup_int(conf, "margin-horizontal", &menumarginh); // majorna.window.margin-horizontal
            config_setting_lookup_int(conf, "x", &xpos); // majorna.window.x
            config_setting_lookup_int(conf, "y", &ypos); // majorna.window.y
            config_setting_lookup_int(conf, "width", &menuwidth); // majorna.window.width
            config_setting_lookup_int(conf, "border", &borderwidth); // majorna.window.border
            config_setting_lookup_int(conf, "managed", &managed); // majorna.window.managed
            config_setting_lookup_int(conf, "grabkeyboard", &grabkeyboard); // majorna.window.grabkeyboard
            config_setting_lookup_int(conf, "monitor", &mon); // majorna.window.monitor
            config_setting_lookup_int(conf, "alpha", &alpha); // majorna.window.alpha

            if (!protocol_override) {
                config_setting_lookup_int(conf, "protocol", &protocol); // majorna.window.protocol
            }
        }
    }

    // load options majorna.properties
    config_setting_t *prop_setting = config_lookup(&cfg, "majorna.properties");
    if (prop_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(prop_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(prop_setting, i);

            // look up
            config_setting_lookup_string(conf, "class", &dest); // majorna.properties.class
            window_class = strdup(dest);

            config_setting_lookup_int(conf, "dock", &dockproperty); // majorna.properties.dock
        }
    }

    // load options majorna.powerline
    config_setting_t *pwl_setting = config_lookup(&cfg, "majorna.powerline");
    if (pwl_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(pwl_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(pwl_setting, i);

            // look up
            config_setting_lookup_int(conf, "promptstyle", &promptpwlstyle); // majorna.powerline.promptstyle
            config_setting_lookup_int(conf, "matchcountstyle", &matchcountpwlstyle); // majorna.powerline.matchcountstyle
            config_setting_lookup_int(conf, "modestyle", &modepwlstyle); // majorna.powerline.modestyle
            config_setting_lookup_int(conf, "capsstyle", &capspwlstyle); // majorna.powerline.capsstyle
            config_setting_lookup_int(conf, "itemstyle", &itempwlstyle); // majorna.powerline.itemstyle
            config_setting_lookup_int(conf, "prompt", &powerlineprompt); // majorna.powerline.prompt
            config_setting_lookup_int(conf, "matchcount", &powerlinecount); // majorna.powerline.matchcount
            config_setting_lookup_int(conf, "mode", &powerlinemode); // majorna.powerline.mode
            config_setting_lookup_int(conf, "caps", &powerlinecaps); // majorna.powerline.caps
            config_setting_lookup_int(conf, "item", &powerlineitems); // majorna.powerline.item
        }
    }

    // load options majorna.center
    config_setting_t *center_setting = config_lookup(&cfg, "majorna.center");
    if (center_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(center_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(center_setting, i);

            config_setting_lookup_int(conf, "width", &centerwidth); // majorna.center.width
        }
    }

    // load options majorna.text
    config_setting_t *text_setting = config_lookup(&cfg, "majorna.text");
    if (text_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(text_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(text_setting, i);

            // look up
            if (config_setting_lookup_string(conf, "font", &dest)) // majorna.text.font
                font = strdup(dest);

            config_setting_lookup_int(conf, "padding", &textpadding); // majorna.text.padding
            config_setting_lookup_int(conf, "normitempadding", &normitempadding); // majorna.text.normitempadding
            config_setting_lookup_int(conf, "selitempadding", &selitempadding); // majorna.text.selitempadding
            config_setting_lookup_int(conf, "priitempadding", &priitempadding); // majorna.text.priitempadding

            if (config_setting_lookup_string(conf, "leftarrow", &dest)) // majorna.text.leftarrow
                leftarrow = strdup(dest);

            if (config_setting_lookup_string(conf, "rightarrow", &dest)) // majorna.text.rightarrow
                rightarrow = strdup(dest);

            if (config_setting_lookup_string(conf, "password", &dest))
                password = strdup(dest);

            if (config_setting_lookup_string(conf, "prompt", &dest))
                prompt = strdup(dest);

            if (config_setting_lookup_string(conf, "pretext", &dest))
                pretext = strdup(dest);

            if (config_setting_lookup_string(conf, "capslockon", &dest))
                capslockontext = strdup(dest);

            if (config_setting_lookup_string(conf, "capslockoff", &dest))
                capslockofftext = strdup(dest);

            if (config_setting_lookup_string(conf, "normal", &dest))
                normtext = strdup(dest);

            if (config_setting_lookup_string(conf, "insert", &dest))
                instext = strdup(dest);

            if (config_setting_lookup_string(conf, "regex", &dest))
                regextext = strdup(dest);

            if (config_setting_lookup_string(conf, "input", &dest))
                input = strdup(dest);
        }
    }

    // load options majorna.alpha
    config_setting_t *alpha_setting = config_lookup(&cfg, "majorna.alpha");
    if (alpha_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(alpha_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(alpha_setting, i);

            // look up
            config_setting_lookup_int(conf, "itemnormfg", &alpha_itemnormfg); // majorna.alpha.itemnormfg
            config_setting_lookup_int(conf, "itemnormbg", &alpha_itemnormbg); // majorna.alpha.itemnormbg

            if (!config_setting_lookup_int(conf, "itemnormfg2", &alpha_itemnormfg2)) // majorna.alpha.itemnormfg2
                config_setting_lookup_int(conf, "itemnormfg", &alpha_itemnormfg2);

            if (!config_setting_lookup_int(conf, "itemnormbg2", &alpha_itemnormbg2)) // majorna.alpha.itemnormbg2
                config_setting_lookup_int(conf, "itemnormbg", &alpha_itemnormbg2);

            config_setting_lookup_int(conf, "itemselfg", &alpha_itemselfg); // majorna.alpha.itemselfg
            config_setting_lookup_int(conf, "itemselbg", &alpha_itemselbg); // majorna.alpha.itemselbg

            if (!config_setting_lookup_int(conf, "itemmarkedfg", &alpha_itemmarkedfg))
                config_setting_lookup_int(conf, "itemselfg", &alpha_itemmarkedfg);

            if (!config_setting_lookup_int(conf, "itemmarkedbg", &alpha_itemmarkedbg))
                config_setting_lookup_int(conf, "itemselbg", &alpha_itemmarkedbg);

            config_setting_lookup_int(conf, "itemnormprifg", &alpha_itemnormprifg); // majorna.alpha.itemnormprifg
            config_setting_lookup_int(conf, "itemnormpribg", &alpha_itemnormpribg); // majorna.alpha.itemnormpribg
            config_setting_lookup_int(conf, "itemselprifg", &alpha_itemselprifg); // majorna.alpha.itemselprifg
            config_setting_lookup_int(conf, "itemselpribg", &alpha_itemselpribg); // majorna.alpha.itemselpribg

            config_setting_lookup_int(conf, "inputfg", &alpha_inputfg); // majorna.alpha.inputfg
            config_setting_lookup_int(conf, "inputbg", &alpha_inputbg); // majorna.alpha.inputbg

            if (!config_setting_lookup_int(conf, "pretextfg", &alpha_pretextfg))
                config_setting_lookup_int(conf, "inputfg", &alpha_pretextfg); // majorna.alpha.pretextfg

            if (!config_setting_lookup_int(conf, "pretextbg", &alpha_pretextbg))
                config_setting_lookup_int(conf, "inputbg", &alpha_pretextbg); // majorna.alpha.pretextbg

            config_setting_lookup_int(conf, "menu", &alpha_menu); // majorna.alpha.menu

            config_setting_lookup_int(conf, "promptfg", &alpha_promptfg); // majorna.alpha.promptfg
            config_setting_lookup_int(conf, "promptbg", &alpha_promptbg); // majorna.alpha.promptbg

            config_setting_lookup_int(conf, "larrowfg", &alpha_larrowfg); // majorna.alpha.larrowfg
            config_setting_lookup_int(conf, "larrowbg", &alpha_larrowbg); // majorna.alpha.larrowbg
            config_setting_lookup_int(conf, "rarrowfg", &alpha_rarrowfg); // majorna.alpha.rarrowfg
            config_setting_lookup_int(conf, "rarrowbg", &alpha_rarrowbg); // majorna.alpha.rarrowbg

            config_setting_lookup_int(conf, "hlnormfg", &alpha_hlnormfg); // majorna.alpha.hlnormfg
            config_setting_lookup_int(conf, "hlnormbg", &alpha_hlnormbg); // majorna.alpha.hlnormbg
            config_setting_lookup_int(conf, "hlselfg", &alpha_hlselfg); // majorna.alpha.hlselfg
            config_setting_lookup_int(conf, "hlselbg", &alpha_hlselbg); // majorna.alpha.hlselbg

            config_setting_lookup_int(conf, "numfg", &alpha_numfg); // majorna.alpha.numfg
            config_setting_lookup_int(conf, "numbg", &alpha_numbg); // majorna.alpha.numbg

            config_setting_lookup_int(conf, "border", &alpha_border); // majorna.alpha.border

            config_setting_lookup_int(conf, "caretfg", &alpha_caretfg); // majorna.alpha.caretfg
            config_setting_lookup_int(conf, "caretbg", &alpha_caretbg); // majorna.alpha.caretbg

            config_setting_lookup_int(conf, "modefg", &alpha_modefg); // majorna.alpha.modefg
            config_setting_lookup_int(conf, "modebg", &alpha_modebg); // majorna.alpha.modebg

            config_setting_lookup_int(conf, "capsfg", &alpha_capsfg); // majorna.alpha.capsfg
            config_setting_lookup_int(conf, "capsbg", &alpha_capsbg); // majorna.alpha.capsbg
        }
    }

    // load options majorna.color
    for (const auto& color_setting : std::vector<config_setting_t*>{
        config_lookup(&cfg, "majorna.color"),
        config_lookup(&cfg, "theme.color"),
    }) {
        if (color_setting != nullptr && loadconfig) {
            for (unsigned int i = 0; i < config_setting_length(color_setting); ++i) {
                config_setting_t *conf = config_setting_get_elem(color_setting, i);

                // items
                if (config_setting_lookup_string(conf, "itemnormfg", &dest))
                    col_itemnormfg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemnormbg", &dest))
                    col_itemnormbg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemnormfg2", &dest))
                    col_itemnormbg2 = strdup(dest);
                else if (config_setting_lookup_string(conf, "itemnormfg", &dest))
                    col_itemnormfg2 = strdup(dest);

                if (config_setting_lookup_string(conf, "itemnormbg2", &dest))
                    col_itemnormbg2 = strdup(dest);
                else if (config_setting_lookup_string(conf, "itemnormbg", &dest))
                    col_itemnormbg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemselfg", &dest))
                    col_itemselfg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemselbg", &dest))
                    col_itemselbg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemmarkedfg", &dest))
                    col_itemmarkedfg = strdup(dest);
                else if (config_setting_lookup_string(conf, "itemselfg", &dest))
                    col_itemmarkedfg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemmarkedbg", &dest))
                    col_itemmarkedbg = strdup(dest);
                else if (config_setting_lookup_string(conf, "itemselbg", &dest))
                    col_itemmarkedbg = strdup(dest);

                // items with priority
                if (config_setting_lookup_string(conf, "itemnormprifg", &dest))
                    col_itemnormprifg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemnormpribg", &dest))
                    col_itemnormpribg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemselprifg", &dest))
                    col_itemselprifg = strdup(dest);

                if (config_setting_lookup_string(conf, "itemselpribg", &dest))
                    col_itemselpribg = strdup(dest);

                // input
                if (config_setting_lookup_string(conf, "inputfg", &dest))
                    col_inputfg = strdup(dest);

                if (config_setting_lookup_string(conf, "inputbg", &dest))
                    col_inputbg = strdup(dest);

                // pretext
                if (config_setting_lookup_string(conf, "pretextfg", &dest))
                    col_pretextfg = strdup(dest);
                else if (config_setting_lookup_string(conf, "inputfg", &dest))
                    col_pretextfg = strdup(dest);

                if (config_setting_lookup_string(conf, "pretextbg", &dest))
                    col_pretextbg = strdup(dest);
                else if (config_setting_lookup_string(conf, "inputfg", &dest))
                    col_pretextbg = strdup(dest);

                // menu
                if (config_setting_lookup_string(conf, "menu", &dest))
                    col_menu = strdup(dest);

                // prompt
                if (config_setting_lookup_string(conf, "promptfg", &dest))
                    col_promptfg = strdup(dest);

                if (config_setting_lookup_string(conf, "promptbg", &dest))
                    col_promptbg = strdup(dest);

                // arrows
                if (config_setting_lookup_string(conf, "larrowfg", &dest))
                    col_larrowfg = strdup(dest);

                if (config_setting_lookup_string(conf, "larrowbg", &dest))
                    col_larrowbg = strdup(dest);

                if (config_setting_lookup_string(conf, "rarrowfg", &dest))
                    col_rarrowfg = strdup(dest);

                if (config_setting_lookup_string(conf, "rarrowbg", &dest))
                    col_rarrowbg = strdup(dest);

                // highlight
                if (config_setting_lookup_string(conf, "hlnormfg", &dest))
                    col_hlnormfg = strdup(dest);

                if (config_setting_lookup_string(conf, "hlnormbg", &dest))
                    col_hlnormbg = strdup(dest);

                if (config_setting_lookup_string(conf, "hlselfg", &dest))
                    col_hlselfg = strdup(dest);

                if (config_setting_lookup_string(conf, "hlselbg", &dest))
                    col_hlselbg = strdup(dest);

                // number
                if (config_setting_lookup_string(conf, "numfg", &dest))
                    col_numfg = strdup(dest);

                if (config_setting_lookup_string(conf, "numbg", &dest))
                    col_numbg = strdup(dest);

                // mode
                if (config_setting_lookup_string(conf, "modefg", &dest))
                    col_modefg = strdup(dest);

                if (config_setting_lookup_string(conf, "modebg", &dest))
                    col_modebg = strdup(dest);

                // caps
                if (config_setting_lookup_string(conf, "capsfg", &dest))
                    col_capsfg = strdup(dest);

                if (config_setting_lookup_string(conf, "capsbg", &dest))
                    col_capsbg = strdup(dest);

                // border
                if (config_setting_lookup_string(conf, "border", &dest))
                    col_border = strdup(dest);

                // caret
                if (config_setting_lookup_string(conf, "caretfg", &dest))
                    col_caretfg = strdup(dest);

                if (config_setting_lookup_string(conf, "caretbg", &dest))
                    col_caretbg = strdup(dest);

                // sgr colors
                for (int i{}; i <= 15; ++i) {
                    std::string key = "sgr" + std::to_string(i);
                    if (config_setting_lookup_string(conf, key.c_str(), &dest)) {
                        textcolors[i] = dest;
                    }
                }

                // coloritems int
                config_setting_lookup_int(conf, "coloritems", &coloritems);
                config_setting_lookup_int(conf, "sgr", &sgr);
            }
        }
    }

    // load options majorna.image
    config_setting_t *img_setting = config_lookup(&cfg, "majorna.image");
    if (img_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(img_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(img_setting, i);

            // look up
            config_setting_lookup_int(conf, "width", &imagewidth); // majorna.image.width
            config_setting_lookup_int(conf, "height", &imageheight); // majorna.image.height
            config_setting_lookup_int(conf, "resize", &imageresize); // majorna.image.resize
            config_setting_lookup_int(conf, "gaps", &imagegaps); // majorna.image.gaps
            config_setting_lookup_int(conf, "position", &imageposition); // majorna.image.position
            config_setting_lookup_int(conf, "type", &imagetype); // majorna.image.type
            config_setting_lookup_int(conf, "cache", &generatecache); // majorna.image.cache
            config_setting_lookup_int(conf, "maxcache", &maxcache); // majorna.image.maxcache
            if (config_setting_lookup_string(conf, "cachedir", &dest)) // majorna.image.cachedir
                cachedir = strdup(dest);
        }
    }

    // load options majorna.file
    config_setting_t *file_setting = config_lookup(&cfg, "majorna.file");
    if (file_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(file_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(file_setting, i);

            // look up
            if (!theme_override) {
                config_setting_lookup_int(conf, "theme", &loadtheme); // majorna.file.theme
            }

            if (!binds_override) {
                config_setting_lookup_int(conf, "binds", &loadbinds); // majorna.file.binds
            }

            config_setting_lookup_int(conf, "xresources", &xresources); // majorna.file.xresources
            config_setting_lookup_int(conf, "global", &xresources); // majorna.file.global

            if (config_setting_lookup_string(conf, "themefile", &dest)) {
                themefile = strdup(dest);
            }

            if (config_setting_lookup_string(conf, "bindsfile", &dest)) {
                bindsfile = strdup(dest);
            }

            if (config_setting_lookup_string(conf, "fifofile", &dest)) {
                fifofile = strdup(dest);
            }

            if (config_setting_lookup_string(conf, "screenshotfile", &dest) && dest != "nullptr" && dest != "NULL") {
                screenshotfile = strdup(dest);
            }

            if (config_setting_lookup_string(conf, "screenshotname", &dest) && dest != "nullptr" && dest != "NULL") {
                screenshotname = strdup(dest);
            }

            if (config_setting_lookup_string(conf, "screenshotdir", &dest) && dest != "nullptr" && dest != "NULL") {
                screenshotdir = strdup(dest);
            }
        }
    }

    // load options majorna.input
    config_setting_t *input_setting = config_lookup(&cfg, "majorna.input");
    if (input_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(input_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(input_setting, i);

            // look up
            config_setting_lookup_int(conf, "fast", &fast); // majorna.input.fast
            config_setting_lookup_int(conf, "type", &type); // majorna.input.type
            config_setting_lookup_int(conf, "password", &passwd); // majorna.input.password
            config_setting_lookup_float(conf, "width", &inputwidth); // majorna.input.width
        }
    }

    // load options majorna.caret
    config_setting_t *caret_setting = config_lookup(&cfg, "majorna.caret");
    if (caret_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(caret_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(caret_setting, i);

            // look up
            config_setting_lookup_int(conf, "width", &caretwidth); // majorna.caret.width
            config_setting_lookup_int(conf, "height", &caretheight); // majorna.caret.height
            config_setting_lookup_int(conf, "padding", &caretpadding); // majorna.caret.padding
        }
    }

    // load options majorna.output
    config_setting_t *output_setting = config_lookup(&cfg, "majorna.output");
    if (output_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(output_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(output_setting, i);

            // look up
            config_setting_lookup_int(conf, "printindex", &printindex); // majorna.output.printindex
            config_setting_lookup_int(conf, "incremental", &incremental); // majorna.output.incremental
        }
    }

    // load options majorna.mode
    config_setting_t *mode_setting = config_lookup(&cfg, "majorna.mode");
    if (mode_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(mode_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(mode_setting, i);

            // look up
            config_setting_lookup_int(conf, "default", &mode); // majorna.mode.default
        }
    }

    // load options majorna.match
    config_setting_t *match_setting = config_lookup(&cfg, "majorna.match");
    if (match_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(match_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(match_setting, i);

            // look up
            config_setting_lookup_int(conf, "sort", &sortmatches); // majorna.match.sort
            config_setting_lookup_int(conf, "casesensitive", &casesensitive); // majorna.match.casesensitive
            config_setting_lookup_int(conf, "fuzzy", &fuzzy); // majorna.match.fuzzy
            config_setting_lookup_int(conf, "regex", &regex); // majorna.match.regex
            config_setting_lookup_int(conf, "preselected", &preselected); // majorna.match.preselected
            config_setting_lookup_int(conf, "mark", &mark); // majorna.match.mark
            config_setting_lookup_string(conf, "delimiters", &dest); // majorna.match.delimiters
            worddelimiters = strdup(dest);
            if (config_setting_lookup_string(conf, "listfile", &dest)) { // majorna.match.listfile
                if (dest != "nullptr" && dest != "NULL") {
                    listfile = strdup(dest);
                }
            }
        }
    }

    // load options majorna.line
    config_setting_t *line_setting = config_lookup(&cfg, "majorna.line");
    if (line_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(line_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(line_setting, i);

            // look up
            config_setting_lookup_int(conf, "itemposition", &itemposition); // majorna.line.itemposition
            config_setting_lookup_int(conf, "height", &lineheight); // majorna.line.height
            config_setting_lookup_int(conf, "lines", &lines); // majorna.line.lines
            config_setting_lookup_int(conf, "columns", &columns); // majorna.line.columns
            config_setting_lookup_int(conf, "overridelines", &overridelines); // majorna.line.overridelines
            config_setting_lookup_int(conf, "overridecolumns", &overridecolumns); // majorna.line.overridecolumns
            config_setting_lookup_int(conf, "minlines", &minlines); // majorna.line.minlines
            config_setting_lookup_int(conf, "indentitems", &indentitems); // majorna.line.indentitems
        }
    }

    // load options majorna.history
    config_setting_t *hist_setting = config_lookup(&cfg, "majorna.history");
    if (hist_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(hist_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(hist_setting, i);

            // look up
            config_setting_lookup_int(conf, "max", &maxhist); // majorna.history.max
            config_setting_lookup_int(conf, "duplicate", &histdup); // majorna.history.duplicate
        }
    }

    // load options majorna.hide
    config_setting_t *hide_setting = config_lookup(&cfg, "majorna.hide");
    if (hide_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(hide_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(hide_setting, i);

            // look up
            config_setting_lookup_int(conf, "input", &hideinput); // majorna.hide.input
            config_setting_lookup_int(conf, "pretext", &hidepretext); // majorna.hide.pretext
            config_setting_lookup_int(conf, "larrow", &hidelarrow); // majorna.hide.larrow
            config_setting_lookup_int(conf, "rarrow", &hiderarrow); // majorna.hide.rarrow
            config_setting_lookup_int(conf, "prompt", &hideprompt); // majorna.hide.prompt
            config_setting_lookup_int(conf, "items",  &hideitem); // majorna.hide.items
            config_setting_lookup_int(conf, "powerline", &hidepowerline); // majorna.hide.powerline
            config_setting_lookup_int(conf, "caret", &hidecaret); // majorna.hide.caret
            config_setting_lookup_int(conf, "highlight", &hidehighlight); // majorna.hide.highlight
            config_setting_lookup_int(conf, "matchcount", &hidematchcount); // majorna.hide.matchcount
            config_setting_lookup_int(conf, "mode", &hidemode); // majorna.hide.mode
            config_setting_lookup_int(conf, "caps", &hidecaps); // majorna.hide.caps
            config_setting_lookup_int(conf, "image", &hideimage); // majorna.hide.image
        }
    }

    // load options majorna.pango
    config_setting_t *pango_setting = config_lookup(&cfg, "majorna.pango");
    if (pango_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(pango_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(pango_setting, i);

            // look up
            config_setting_lookup_int(conf, "item", &pango_item); // majorna.pango.input
            config_setting_lookup_int(conf, "prompt", &pango_prompt); // majorna.pango.rarrow
            config_setting_lookup_int(conf, "input", &pango_input); // majorna.pango.input
            config_setting_lookup_int(conf, "pretext", &pango_pretext); // majorna.pango.pretext
            config_setting_lookup_int(conf, "leftarrow", &pango_leftarrow); // majorna.pango.leftarrow
            config_setting_lookup_int(conf, "rightarrow", &pango_rightarrow); // majorna.pango.rightarrow
            config_setting_lookup_int(conf, "numbers", &pango_numbers); // majorna.pango.numbers
            config_setting_lookup_int(conf, "mode", &pango_mode); // majorna.pango.mode
            config_setting_lookup_int(conf, "caps", &pango_caps); // majorna.pango.caps
            config_setting_lookup_int(conf, "password", &pango_password); // majorna.pango.mode
        }
    }

    if (!bind_init()) {
        config_setting_t *key_setting = config_lookup(&cfg, "majorna.keys");
        if (key_setting != nullptr && loadconfig) {
            int nmode{0};

            for (unsigned int i = 0; i < config_setting_length(key_setting); ++i) {
                config_setting_t *conf = config_setting_get_elem(key_setting, i);

                // look up
                config_setting_lookup_string(conf, "modifier", &dest);

#if X11
                Key key{};
#endif
#if WAYLAND
                WlKey wl_key{};
#endif

#if X11
                for (const auto& it : ml) {
                    if (it.mod == dest) {
                        key.mod = it.modifier;
                    }
                }
#endif

#if WAYLAND
                for (const auto& it : wml) {
                    if (it.mod == dest) {
                        wl_key.mod = it.modifier;
                    }
                }
#endif

                if (config_setting_lookup_int(conf, "mode", &nmode)) {
                    if (nmode < 0) nmode = -1;
                    if (nmode > 1) nmode = 1;

#if X11
                    key.mode = nmode;
#endif
#if WAYLAND
                    wl_key.mode = nmode;
#endif
                }

                config_setting_lookup_string(conf, "key", &dest);

#if X11
                for (const auto& it : kl) {
                    if (it.key == dest) {
                        key.keysym = it.keysym;
                    }
                }
#endif

#if WAYLAND
                for (const auto& it : wkl) {
                    if (it.key == dest) {
                        wl_key.keysym = it.keysym;
                    }
                }
#endif

                config_setting_lookup_string(conf, "function", &dest);

                for (const auto& it : fl) {
                    if (it.function == dest) {
#if X11
                        key.func = it.func;
#endif
#if WAYLAND
                        wl_key.func = it.func;
#endif
                    }
                }

                config_setting_lookup_string(conf, "argument", &dest);

                if (strdup(dest)) {
#if X11
                    key.arg.i = std::stoi(dest);
#endif
#if WAYLAND
                    wl_key.arg.i = std::stoi(dest);
#endif
                }

                config_setting_lookup_bool(conf, "forceinsertmode", reinterpret_cast<int*>(forceinsertmode));
                config_setting_lookup_bool(conf, "ignoreglobalkeys", reinterpret_cast<int*>(&ctx.ignore_global_keys));

#if X11
                ckeys.emplace_back(key);
#endif
#if WAYLAND
                wl_ckeys.emplace_back(wl_key);
#endif
            }
        }

        // load options majorna.mouse
        config_setting_t *mouse_setting = config_lookup(&cfg, "majorna.mouse");
        if (mouse_setting != nullptr && loadconfig) {
            for (unsigned int i = 0; i < config_setting_length(mouse_setting); ++i) {
                config_setting_t *conf = config_setting_get_elem(mouse_setting, i);

#if X11
                Mouse mouse{};
#endif
#if WAYLAND
                WlMouse wl_mouse{};
#endif

                config_setting_lookup_string(conf, "click", &dest);

                for (const auto& it : ctp) {
                    if (it.tclick == dest) {
#if X11
                        mouse.click = it.click;
#endif
#if WAYLAND
                        wl_mouse.click = it.click;
#endif
                    }
                }

                config_setting_lookup_string(conf, "button", &dest);

#if X11
                for (const auto& it : btp) {
                    if (it.click == dest) {
                        mouse.button = it.button;
                    }
                }
#endif

#if WAYLAND
                for (const auto& it : w_btp) {
                    if (it.click == dest) {
                        wl_mouse.button = it.button;
                    }
                }
#endif

                config_setting_lookup_string(conf, "function", &dest);

                for (const auto& it : fl) {
                    if (it.function == dest) {
#if X11
                        mouse.func = it.func;
#endif
#if WAYLAND
                        wl_mouse.func = it.func;
#endif
                    }
                }

                config_setting_lookup_string(conf, "argument", &dest);

                if (strdup(dest)) {
#if X11
                    mouse.arg.i = std::stoi(dest);
#endif
#if WAYLAND
                    wl_mouse.arg.i = std::stoi(dest);
#endif
                }

                config_setting_lookup_bool(conf, "ignoreglobalmouse", reinterpret_cast<int*>(&ctx.ignore_global_mouse));
                config_setting_lookup_int(conf, "scrolldistance", &scrolldistance);

#if X11
                cbuttons.emplace_back(mouse);
#endif
#if WAYLAND
                wl_cbuttons.emplace_back(wl_mouse);
#endif
            }
        }
    }

    // we're done with this config
    config_destroy(&cfg);

    // load the theme now
    if (loadtheme) {
        theme_load();
    }
}

void theme_load() {
    if (!loadtheme)
        return;

    std::string tmfile{};
    const char *dest;

    // get path for configuration file
    if (tmfile.empty()) {
        const char* xdg_conf = getenv("XDG_CONFIG_HOME");
        if (!xdg_conf) {
            // ~/.config/majorna/theme.conf
            const char* home = getenv("HOME");
            if (!home) {
                die("majorna: HOME environment variable is not set");
            }
            tmfile = std::string(home) + "/.config/majorna/theme.conf";
        } else {
            // XDG_CONFIG_HOME is set, so let's use that instead
            tmfile = std::string(xdg_conf) + "/majorna/theme.conf";
        }
    } else { // custom config path
        tmfile = themefile;
    }

    if (!std::filesystem::is_regular_file(tmfile)) {
        loadtheme = 0;
    }

    // init config
    config_t cfg;
    config_init(&cfg);

    // attempt to read theme
    if (!config_read_file(&cfg, tmfile.c_str())) {
        fprintf(stderr, "majorna: Invalid theme.\n"); // invalid configuration, but let's try to read it anyway
    }

    // load options theme.window
    config_setting_t *window_setting = config_lookup(&cfg, "theme.window");
    if (window_setting != nullptr) {
        // look up window entries
        for (unsigned int i = 0; i < config_setting_length(window_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(window_setting, i);

            config_setting_lookup_int(conf, "position", &menuposition); // theme.window.menuposition
            config_setting_lookup_int(conf, "paddingv", &menupaddingv); // theme.window.paddingv
            config_setting_lookup_int(conf, "paddingh", &menupaddingh); // theme.window.paddingh
            config_setting_lookup_int(conf, "padding-vertical", &menupaddingv); // theme.window.padding-vertical
            config_setting_lookup_int(conf, "padding-horizontal", &menupaddingh); // theme.window.padding-horizontal
            config_setting_lookup_int(conf, "margin-vertical", &menumarginv); // theme.window.margin-vertical
            config_setting_lookup_int(conf, "margin-horizontal", &menumarginh); // theme.window.margin-horizontal
            config_setting_lookup_int(conf, "x", &xpos); // theme.window.x
            config_setting_lookup_int(conf, "y", &xpos); // theme.window.y
            config_setting_lookup_int(conf, "width", &menuwidth); // theme.window.width
            config_setting_lookup_int(conf, "border", &borderwidth); // theme.window.border
            config_setting_lookup_int(conf, "alpha", &alpha); // theme.window.alpha
        }
    }

    // load options theme.caret
    config_setting_t *caret_setting = config_lookup(&cfg, "theme.caret");
    if (caret_setting != nullptr && loadconfig) {
        for (unsigned int i = 0; i < config_setting_length(caret_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(caret_setting, i);

            // look up
            config_setting_lookup_int(conf, "width", &caretwidth); // theme.caret.width
            config_setting_lookup_int(conf, "height", &caretheight); // theme.caret.height
            config_setting_lookup_int(conf, "padding", &caretpadding); // theme.caret.padding
        }
    }

    // load options theme.powerline
    config_setting_t *pwl_setting = config_lookup(&cfg, "theme.powerline");
    if (pwl_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(pwl_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(pwl_setting, i);

            // look up
            config_setting_lookup_int(conf, "promptstyle", &promptpwlstyle); // theme.powerline.promptstyle
            config_setting_lookup_int(conf, "matchcountstyle", &matchcountpwlstyle); // theme.powerline.matchcountstyle
            config_setting_lookup_int(conf, "modestyle", &modepwlstyle); // theme.powerline.modestyle
            config_setting_lookup_int(conf, "capsstyle", &capspwlstyle); // theme.powerline.capsstyle
            config_setting_lookup_int(conf, "itemstyle", &itempwlstyle); // theme.powerline.itemstyle
            config_setting_lookup_int(conf, "prompt", &powerlineprompt); // theme.powerline.prompt
            config_setting_lookup_int(conf, "matchcount", &powerlinecount); // theme.powerline.matchcount
            config_setting_lookup_int(conf, "mode", &powerlinemode); // theme.powerline.mode
            config_setting_lookup_int(conf, "caps", &powerlinecaps); // theme.powerline.caps
            config_setting_lookup_int(conf, "item", &powerlineitems); // theme.powerline.item
        }
    }

    // load options theme.center
    config_setting_t *center_setting = config_lookup(&cfg, "theme.center");
    if (center_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(center_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(center_setting, i);

            config_setting_lookup_int(conf, "width", &centerwidth); // theme.center.width
        }
    }

    // load options theme.text
    config_setting_t *text_setting = config_lookup(&cfg, "theme.text");
    if (text_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(text_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(text_setting, i);

            // look up
            if (config_setting_lookup_string(conf, "font", &dest)) // theme.text.font
                font = strdup(dest);

            config_setting_lookup_int(conf, "padding", &textpadding); // theme.text.padding
            config_setting_lookup_int(conf, "normitempadding", &normitempadding); // theme.text.normitempadding
            config_setting_lookup_int(conf, "selitempadding", &selitempadding); // theme.text.selitempadding
            config_setting_lookup_int(conf, "priitempadding", &priitempadding); // theme.text.priitempadding

            if (config_setting_lookup_string(conf, "leftarrow", &dest)) // theme.text.leftarrow
                leftarrow = strdup(dest);

            if (config_setting_lookup_string(conf, "rightarrow", &dest)) // theme.text.rightarrow
                rightarrow = strdup(dest);

            if (config_setting_lookup_string(conf, "password", &dest))
                password = strdup(dest);

            if (config_setting_lookup_string(conf, "prompt", &dest))
                prompt = strdup(dest);

            if (config_setting_lookup_string(conf, "pretext", &dest))
                pretext = strdup(dest);

            if (config_setting_lookup_string(conf, "capslockon", &dest))
                capslockontext = strdup(dest);

            if (config_setting_lookup_string(conf, "capslockoff", &dest))
                capslockofftext = strdup(dest);

            if (config_setting_lookup_string(conf, "regex", &dest))
                regextext = strdup(dest);

            if (config_setting_lookup_string(conf, "input", &dest))
                input = strdup(dest);
        }
    }

    // load options theme.alpha
    config_setting_t *alpha_setting = config_lookup(&cfg, "theme.alpha");
    if (alpha_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(alpha_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(alpha_setting, i);

            // look up
            config_setting_lookup_int(conf, "itemnormfg", &alpha_itemnormfg); // theme.alpha.itemnormfg
            config_setting_lookup_int(conf, "itemnormbg", &alpha_itemnormbg); // theme.alpha.itemnormbg

            if (!config_setting_lookup_int(conf, "itemnormfg2", &alpha_itemnormfg2)) // theme.alpha.itemnormfg2
                config_setting_lookup_int(conf, "itemnormfg", &alpha_itemnormfg2);

            if (!config_setting_lookup_int(conf, "itemnormbg2", &alpha_itemnormbg2)) // theme.alpha.itemnormbg2
                config_setting_lookup_int(conf, "itemnormbg", &alpha_itemnormbg2);

            config_setting_lookup_int(conf, "itemselfg", &alpha_itemselfg); // theme.alpha.itemselfg
            config_setting_lookup_int(conf, "itemselbg", &alpha_itemselbg); // theme.alpha.itemselbg

            if (!config_setting_lookup_int(conf, "itemmarkedfg", &alpha_itemmarkedfg))
                config_setting_lookup_int(conf, "itemselfg", &alpha_itemmarkedfg);

            if (!config_setting_lookup_int(conf, "itemmarkedbg", &alpha_itemmarkedbg))
                config_setting_lookup_int(conf, "itemselbg", &alpha_itemmarkedbg);

            config_setting_lookup_int(conf, "itemnormprifg", &alpha_itemnormprifg); // theme.alpha.itemnormprifg
            config_setting_lookup_int(conf, "itemnormpribg", &alpha_itemnormpribg); // theme.alpha.itemnormpribg
            config_setting_lookup_int(conf, "itemselprifg", &alpha_itemselprifg); // theme.alpha.itemselprifg
            config_setting_lookup_int(conf, "itemselpribg", &alpha_itemselpribg); // theme.alpha.itemselpribg

            config_setting_lookup_int(conf, "inputfg", &alpha_inputfg); // theme.alpha.inputfg
            config_setting_lookup_int(conf, "inputbg", &alpha_inputbg); // theme.alpha.inputbg

            if (!config_setting_lookup_int(conf, "pretextfg", &alpha_pretextfg))
                config_setting_lookup_int(conf, "inputfg", &alpha_pretextfg); // theme.alpha.pretextfg

            if (!config_setting_lookup_int(conf, "pretextbg", &alpha_pretextbg))
                config_setting_lookup_int(conf, "inputbg", &alpha_pretextbg); // theme.alpha.pretextbg

            config_setting_lookup_int(conf, "menu", &alpha_menu); // theme.alpha.menu

            config_setting_lookup_int(conf, "promptfg", &alpha_promptfg); // theme.alpha.promptfg
            config_setting_lookup_int(conf, "promptbg", &alpha_promptbg); // theme.alpha.promptbg

            config_setting_lookup_int(conf, "larrowfg", &alpha_larrowfg); // theme.alpha.larrowfg
            config_setting_lookup_int(conf, "larrowbg", &alpha_larrowbg); // theme.alpha.larrowbg
            config_setting_lookup_int(conf, "rarrowfg", &alpha_rarrowfg); // theme.alpha.rarrowfg
            config_setting_lookup_int(conf, "rarrowbg", &alpha_rarrowbg); // theme.alpha.rarrowbg

            config_setting_lookup_int(conf, "hlnormfg", &alpha_hlnormfg); // theme.alpha.hlnormfg
            config_setting_lookup_int(conf, "hlnormbg", &alpha_hlnormbg); // theme.alpha.hlnormbg
            config_setting_lookup_int(conf, "hlselfg", &alpha_hlselfg); // theme.alpha.hlselfg
            config_setting_lookup_int(conf, "hlselbg", &alpha_hlselbg); // theme.alpha.hlselbg

            config_setting_lookup_int(conf, "numfg", &alpha_numfg); // theme.alpha.numfg
            config_setting_lookup_int(conf, "numbg", &alpha_numbg); // theme.alpha.numbg

            config_setting_lookup_int(conf, "border", &alpha_border); // theme.alpha.border

            config_setting_lookup_int(conf, "caretfg", &alpha_caretfg); // theme.alpha.caretfg
            config_setting_lookup_int(conf, "caretbg", &alpha_caretbg); // theme.alpha.caretbg

            config_setting_lookup_int(conf, "modefg", &alpha_modefg); // theme.alpha.modefg
            config_setting_lookup_int(conf, "modebg", &alpha_modebg); // theme.alpha.modebg

            config_setting_lookup_int(conf, "capsfg", &alpha_capsfg); // theme.alpha.capsfg
            config_setting_lookup_int(conf, "capsbg", &alpha_capsbg); // theme.alpha.capsbg
        }
    }

    // load options theme.color
    config_setting_t *color_setting = config_lookup(&cfg, "theme.color");
    if (color_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(color_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(color_setting, i);

            if (config_setting_lookup_string(conf, "itemnormfg", &dest))
                col_itemnormfg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemnormbg", &dest))
                col_itemnormbg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemnormfg2", &dest))
                col_itemnormbg2 = strdup(dest);
            else if (config_setting_lookup_string(conf, "itemnormfg", &dest))
                col_itemnormfg2 = strdup(dest);

            if (config_setting_lookup_string(conf, "itemnormbg2", &dest))
                col_itemnormbg2 = strdup(dest);
            else if (config_setting_lookup_string(conf, "itemnormbg", &dest))
                col_itemnormbg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemselfg", &dest))
                col_itemselfg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemselbg", &dest))
                col_itemselbg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemmarkedfg", &dest))
                col_itemmarkedfg = strdup(dest);
            else if (config_setting_lookup_string(conf, "itemselfg", &dest))
                col_itemmarkedfg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemmarkedbg", &dest))
                col_itemmarkedbg = strdup(dest);
            else if (config_setting_lookup_string(conf, "itemselbg", &dest))
                col_itemmarkedbg = strdup(dest);

            // items with priority
            if (config_setting_lookup_string(conf, "itemnormprifg", &dest))
                col_itemnormprifg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemnormpribg", &dest))
                col_itemnormpribg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemselprifg", &dest))
                col_itemselprifg = strdup(dest);

            if (config_setting_lookup_string(conf, "itemselpribg", &dest))
                col_itemselpribg = strdup(dest);

            // input
            if (config_setting_lookup_string(conf, "inputfg", &dest))
                col_inputfg = strdup(dest);

            if (config_setting_lookup_string(conf, "inputbg", &dest))
                col_inputbg = strdup(dest);

            // pretext
            if (config_setting_lookup_string(conf, "pretextfg", &dest))
                col_pretextfg = strdup(dest);
            else if (config_setting_lookup_string(conf, "inputfg", &dest))
                col_pretextfg = strdup(dest);

            if (config_setting_lookup_string(conf, "pretextbg", &dest))
                col_pretextbg = strdup(dest);
            else if (config_setting_lookup_string(conf, "inputfg", &dest))
                col_pretextbg = strdup(dest);

            // menu
            if (config_setting_lookup_string(conf, "menu", &dest))
                col_menu = strdup(dest);

            // prompt
            if (config_setting_lookup_string(conf, "promptfg", &dest))
                col_promptfg = strdup(dest);

            if (config_setting_lookup_string(conf, "promptbg", &dest))
                col_promptbg = strdup(dest);

            // arrows
            if (config_setting_lookup_string(conf, "larrowfg", &dest))
                col_larrowfg = strdup(dest);

            if (config_setting_lookup_string(conf, "larrowbg", &dest))
                col_larrowbg = strdup(dest);

            if (config_setting_lookup_string(conf, "rarrowfg", &dest))
                col_rarrowfg = strdup(dest);

            if (config_setting_lookup_string(conf, "rarrowbg", &dest))
                col_rarrowbg = strdup(dest);

            // highlight
            if (config_setting_lookup_string(conf, "hlnormfg", &dest))
                col_hlnormfg = strdup(dest);

            if (config_setting_lookup_string(conf, "hlnormbg", &dest))
                col_hlnormbg = strdup(dest);

            if (config_setting_lookup_string(conf, "hlselfg", &dest))
                col_hlselfg = strdup(dest);

            if (config_setting_lookup_string(conf, "hlselbg", &dest))
                col_hlselbg = strdup(dest);

            // number
            if (config_setting_lookup_string(conf, "numfg", &dest))
                col_numfg = strdup(dest);

            if (config_setting_lookup_string(conf, "numbg", &dest))
                col_numbg = strdup(dest);

            // mode
            if (config_setting_lookup_string(conf, "modefg", &dest))
                col_modefg = strdup(dest);

            if (config_setting_lookup_string(conf, "modebg", &dest))
                col_modebg = strdup(dest);

            // caps
            if (config_setting_lookup_string(conf, "capsfg", &dest))
                col_capsfg = strdup(dest);

            if (config_setting_lookup_string(conf, "capsbg", &dest))
                col_capsbg = strdup(dest);

            // border
            if (config_setting_lookup_string(conf, "border", &dest))
                col_border = strdup(dest);

            // caret
            if (config_setting_lookup_string(conf, "caretfg", &dest))
                col_caretfg = strdup(dest);

            if (config_setting_lookup_string(conf, "caretbg", &dest))
                col_caretbg = strdup(dest);

            // sgr colors
            for (int i{}; i <= 15; ++i) {
                std::string key = "sgr" + std::to_string(i);
                if (config_setting_lookup_string(conf, key.c_str(), &dest)) {
                    textcolors[i] = dest;
                }
            }

            // coloritems int
            config_setting_lookup_int(conf, "coloritems", &coloritems);
            config_setting_lookup_int(conf, "sgr", &sgr);
        }
    }

    // load options theme.image
    config_setting_t *img_setting = config_lookup(&cfg, "theme.image");
    if (img_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(img_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(img_setting, i);

            // look up
            config_setting_lookup_int(conf, "width", &imagewidth); // theme.image.width
            config_setting_lookup_int(conf, "height", &imageheight); // theme.image.height
            config_setting_lookup_int(conf, "gaps", &imagegaps); // theme.image.gaps
            config_setting_lookup_int(conf, "position", &imageposition); // theme.image.position
        }
    }

    // load options theme.mode
    config_setting_t *mode_setting = config_lookup(&cfg, "theme.mode");
    if (mode_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(mode_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(mode_setting, i);

            config_setting_lookup_string(conf, "normal_text", &dest); // theme.mode.normal_text
            normtext = strdup(dest);

            config_setting_lookup_string(conf, "insert_text", &dest); // theme.mode.insert_text
            instext = strdup(dest);
        }
    }

    // load options theme.line
    config_setting_t *line_setting = config_lookup(&cfg, "theme.line");
    if (line_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(line_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(line_setting, i);

            // look up
            config_setting_lookup_int(conf, "itemposition", &itemposition); // theme.line.itemposition
            config_setting_lookup_int(conf, "height", &lineheight); // theme.line.height
            config_setting_lookup_int(conf, "indentitems", &indentitems); // theme.line.indentitems
        }
    }

    // load options theme.hide
    config_setting_t *hide_setting = config_lookup(&cfg, "theme.hide");
    if (hide_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(hide_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(hide_setting, i);

            // look up
            config_setting_lookup_int(conf, "input", &hideinput); // theme.hide.input
            config_setting_lookup_int(conf, "pretext", &hidepretext); // theme.hide.pretext
            config_setting_lookup_int(conf, "larrow", &hidelarrow); // theme.hide.larrow
            config_setting_lookup_int(conf, "rarrow", &hiderarrow); // theme.hide.rarrow
            config_setting_lookup_int(conf, "prompt", &hideprompt); // theme.hide.prompt
            config_setting_lookup_int(conf, "items",  &hideitem); // theme.hide.items
            config_setting_lookup_int(conf, "powerline", &hidepowerline); // theme.hide.powerline
            config_setting_lookup_int(conf, "caret", &hidecaret); // theme.hide.caret
            config_setting_lookup_int(conf, "highlight", &hidehighlight); // theme.hide.highlight
            config_setting_lookup_int(conf, "matchcount", &hidematchcount); // theme.hide.matchcount
            config_setting_lookup_int(conf, "mode", &hidemode); // theme.hide.mode
            config_setting_lookup_int(conf, "caps", &hidecaps); // theme.hide.caps
            config_setting_lookup_int(conf, "image", &hideimage); // theme.hide.image
        }
    }

    // load options theme.pango
    config_setting_t *pango_setting = config_lookup(&cfg, "theme.pango");
    if (pango_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(pango_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(pango_setting, i);

            // look up
            config_setting_lookup_int(conf, "item", &pango_item); // theme.pango.input
            config_setting_lookup_int(conf, "prompt", &pango_prompt); // theme.pango.rarrow
            config_setting_lookup_int(conf, "input", &pango_input); // theme.pango.input
            config_setting_lookup_int(conf, "pretext", &pango_pretext); // theme.pango.pretext
            config_setting_lookup_int(conf, "leftarrow", &pango_leftarrow); // theme.pango.leftarrow
            config_setting_lookup_int(conf, "rightarrow", &pango_rightarrow); // theme.pango.rightarrow
            config_setting_lookup_int(conf, "numbers", &pango_numbers); // theme.pango.numbers
            config_setting_lookup_int(conf, "mode", &pango_mode); // theme.pango.mode
            config_setting_lookup_int(conf, "caps", &pango_caps); // theme.pango.caps
            config_setting_lookup_int(conf, "password", &pango_password); // theme.pango.mode
        }
    }

    config_setting_t *file_setting = config_lookup(&cfg, "theme.file");
    if (file_setting != nullptr) {
        for (unsigned int i = 0; i < config_setting_length(file_setting); ++i) {
            config_setting_t *conf = config_setting_get_elem(file_setting, i);

            config_setting_lookup_int(conf, "global", &xresources); // theme.file.global
        }
    }

    // we're done here
    config_destroy(&cfg);
}
#endif
