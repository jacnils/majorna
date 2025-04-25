/* See LICENSE file for copyright and license details. */

#include <options.hpp>
#include <majorna.hpp>
#include <macros.hpp>
#include <match.hpp>
#include <draw.hpp>
#include <wl/wayland.hpp>
#include <wl/shm.hpp>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <sys/poll.h>

struct wl_buffer *create_buffer(struct state *state) {
    int32_t width = state->width;
    int32_t height = state->height;

    uint32_t stride = width * 4;
    size_t siz = stride * height;

    int fd = create_shm_file(siz);
    if (fd == -1) {
        close(fd);
        throw std::runtime_error("Failed to create shared memory file");
    }

    void *data = mmap(nullptr, siz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        close(fd);
        return nullptr;
    }

	struct wl_shm_pool *pool = wl_shm_create_pool(state->shm, fd, siz);

	struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0,
			width, height, stride, WL_SHM_FORMAT_ARGB8888);

    wl_shm_pool_destroy(pool);

    wl_buffer_add_listener(buffer, &buffer_listener, state);

    state->data = data;

    return buffer;
}

/* I fucking hate this code, but I'm not sure how else to do it.
 *
 * PLEASE submit a patch if you have an improvement.
 */
int is_correct_modifier(struct state *state, const std::string& modifier) {
    bool alt_pressed = xkb_state_mod_name_is_active(state->xkb_state, XKB_MOD_NAME_ALT, static_cast<xkb_state_component>(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED));
    bool shift_pressed = xkb_state_mod_name_is_active(state->xkb_state, XKB_MOD_NAME_SHIFT, static_cast<xkb_state_component>(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED));
    bool ctrl_pressed = xkb_state_mod_name_is_active(state->xkb_state, XKB_MOD_NAME_CTRL, static_cast<xkb_state_component>(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED));
    bool logo_pressed = xkb_state_mod_name_is_active(state->xkb_state, XKB_MOD_NAME_LOGO, static_cast<xkb_state_component>(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED));

    std::unordered_map<std::string, std::function<bool()>> modifier_conditions = {
        {"WL_CtrlShift", [&]() { return shift_pressed && ctrl_pressed; }},
        {"WL_CtrlShiftSuper", [&]() { return shift_pressed && ctrl_pressed && logo_pressed; }},
        {"WL_CtrlSuper", [&]() { return ctrl_pressed && logo_pressed; }},
        {"WL_CtrlAlt", [&]() { return alt_pressed && ctrl_pressed; }},
        {"WL_CtrlAltShift", [&]() { return ctrl_pressed && alt_pressed && shift_pressed; }},
        {"WL_CtrlAltShiftSuper", [&]() { return ctrl_pressed && alt_pressed && shift_pressed && logo_pressed; }},
        {"WL_CtrlAltSuper", [&]() { return ctrl_pressed && alt_pressed && logo_pressed; }},
        {"WL_AltShift", [&]() { return alt_pressed && shift_pressed; }},
        {"WL_Shift", [&]() { return shift_pressed; }},
        {"WL_Ctrl", [&]() { return ctrl_pressed; }},
        {"WL_Alt", [&]() { return alt_pressed; }},
        {"WL_Super", [&]() { return logo_pressed; }},
        {"WL_None", [&]() { return !alt_pressed && !shift_pressed && !ctrl_pressed && !logo_pressed; }}
    };

    auto it = modifier_conditions.find(modifier);
    if (it != modifier_conditions.end() && it->second()) {
        return true;
    }

    return false;
}

/* This function is pretty garbage. However I don't feel like implementing all the garbage necessary to paste properly.
 * If anyone wants to do it, feel free to pull request.
 */
char *wl_clipboard() {
    FILE *fp;

    char output_text[1024];
    char *clipboard = static_cast<char*>(malloc(sizeof(output_text)));

    clipboard[0] = '\0';

    fp = popen("which wl-paste > /dev/null && wl-paste -t text/plain", "r");

    if (fp == nullptr) {
        fprintf(stderr, "majorna: Failed to open command\n");
        return nullptr;
    }

    while (fgets(output_text, sizeof(output_text), fp) != nullptr) {
        strcat(clipboard, output_text);
    }

    pclose(fp);

    return clipboard;
}

void paste_wl() {
    char *p, *q;

    p = wl_clipboard();

    insert(p, (q = strchr(p, '\n')) ? q - p : (ssize_t)strlen(p)); // insert selection

    draw_menu();
}

void keypress_wl(struct state *state, enum wl_keyboard_key_state key_state, xkb_keysym_t sym) {
    int i = 0;
    char buf[8];

    if (key_state != WL_KEYBOARD_KEY_STATE_PRESSED) { // Only activate on press, not release
        return;
    }

    if (xkb_keysym_to_lower(sym) == wlhkeys[0].keysym && !is_correct_modifier(state, wlhkeys[0].mod) && wlhkeys[0].func) {
        wlhkeys[0].func(wlhkeys[0].arg);
    }

    for (auto& it : wl_keys) {
        if (ctx.ignore_global_keys) break;

        if (xkb_keysym_to_lower(sym) == it.keysym && !is_correct_modifier(state, it.mod) && it.func) {
            if ((it.mode && ctx.mode) || it.mode == -1) {
                it.func(it.arg);
                return;
            } else if (!it.mode && !ctx.mode) {
                it.func(it.arg);
            }
        }
    }

    for (auto& it : wl_ckeys) {
        if (ctx.ignore_conf_keys) break;

        if (xkb_keysym_to_lower(sym) == it.keysym && !is_correct_modifier(state, it.mod) && it.func) {
            if ((it.mode && ctx.mode) || it.mode == -1) {
                it.func(it.arg);
                return;
            } else if (!it.mode && !ctx.mode) {
                it.func(it.arg);
            }
        }
    }

    if (xkb_keysym_to_lower(sym) == XKB_KEY_Escape) {
        return;
    }

    if (!type || !ctx.mode) {
        return;
    }

    if (xkb_keysym_to_utf8(sym, buf, 8)) {
        if (ctx.allow_input) {
            insert(buf, strnlen(buf, 8));
        } else {
            ctx.allow_input = !ctx.allow_input;
        }
    }

    draw_menu();
}

void keyboard_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
	struct state* state = static_cast<struct state*>(data);
    int ocapslockstate = ctx.caps_state;

	xkb_state_update_mask(state->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);

    if (xkb_state_mod_name_is_active(state->xkb_state, XKB_MOD_NAME_CAPS, XKB_STATE_MODS_EFFECTIVE)) {
        ctx.caps_state = 1;
    } else {
        ctx.caps_state = 0;
    }

    if (ocapslockstate != ctx.caps_state) {
        strncpy(strings.caps_text, ctx.caps_state ? capslockontext.c_str() : capslockofftext.c_str(), 15);
        draw_menu();
    }
}

void keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard, int32_t rate, int32_t delay) {
	struct state* state = static_cast<struct state*>(data);

	state->delay = delay;

	if (rate > 0) {
		state->period = 1000 / rate;
	} else {
		state->period = -1;
	}
}

void keyboard_repeat(struct state *state) {
    keypress_wl(state, state->repeat_key_state, state->repeat_sym);

    struct itimerspec spec = { 0 };

    spec.it_value.tv_sec = state->period / 1000;
	spec.it_value.tv_nsec = (state->period % 1000) * 1000000l;

	timerfd_settime(state->timer, 0, &spec, nullptr);
}

void keyboard_key(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t _key_state) {
	struct state* state = static_cast<struct state*>(data);

	wl_keyboard_key_state key_state = static_cast<wl_keyboard_key_state>(_key_state);

    strncpy(strings.caps_text, ctx.caps_state ? capslockontext.c_str() : capslockofftext.c_str(), 15);

	xkb_keysym_t sym = xkb_state_key_get_one_sym(state->xkb_state, key + 8);
	keypress_wl(state, key_state, sym);

	if (key_state == WL_KEYBOARD_KEY_STATE_PRESSED && state->period >= 0) {
		state->repeat_key_state = key_state;
		state->repeat_sym = sym;

		struct itimerspec spec = { 0 };

		spec.it_value.tv_sec = state->delay / 1000;
		spec.it_value.tv_nsec = (state->delay % 1000) * 1000000l;

		timerfd_settime(state->timer, 0, &spec, nullptr);
	} else if (key_state == WL_KEYBOARD_KEY_STATE_RELEASED) {
		struct itimerspec spec = { 0 };

		timerfd_settime(state->timer, 0, &spec, nullptr);
	}
}

void keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size) {
	struct state* state = static_cast<struct state*>(data);
	if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
		close(fd);
        exit(1);
		return;
	}
	char *map_shm = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0));
	if (map_shm == MAP_FAILED) {
		close(fd);
        die("MAP_FAILED");
		return;
	}
	state->xkb_keymap = xkb_keymap_new_from_string(state->xkb_context, map_shm, XKB_KEYMAP_FORMAT_TEXT_V1, static_cast<xkb_keymap_compile_flags>(0));
	munmap(map_shm, size);
	close(fd);
	state->xkb_state = xkb_state_new(state->xkb_keymap);
}

void buttonpress_wl(uint32_t button, double ex, double ey) {
    int x = 0;
    int y = 0;
    int w;
    int h = ctx.item_height;
    int xpad = 0;
    int item_num = 0;
    int yp = 0;
    unsigned int i, click;
    struct item *item;

    if (ex == 0 && ey == 0) {
        return; // While it is possible to click at this position, usually it means we're outside the window area.
    }

    if (!hidepowerline) {
        x = xpad = ctx.powerline_width;
    }

    x += menumarginh;

    int larroww = 0;
    int rarroww = 0;
    int numberw = 0;
    int modew = 0;
    int capsw = 0;

    if (!hidelarrow) larroww = pango_leftarrow ? TEXTWM(leftarrow) : TEXTW(leftarrow);
    if (!hiderarrow) rarroww = pango_rightarrow ? TEXTWM(rightarrow) : TEXTW(rightarrow);
    if (!hidematchcount) numberw = pango_numbers ? TEXTWM(strings.number_text) : TEXTW(strings.number_text);
    if (!hidemode) modew = pango_mode ? TEXTWM(strings.mode_text) : TEXTW(strings.mode_text);
    if (!hidecaps) capsw = pango_caps ? TEXTWM(strings.caps_text) : TEXTW(strings.caps_text);

    if (!strcmp(strings.caps_text, ""))
        capsw = 0;

    if ((hideprompt && hideinput && hidemode && hidematchcount && hidecaps) && lines) {
        yp = 1;
    } else if (!itemposition && lines && ey <= h + menumarginv && ey >= menumarginv) {
        yp = 1;
    } else if (itemposition && lines && ey >= (ctx.win_height - h) + menumarginv) {
        yp = 1;
    } else if (!lines) {
        yp = 1;
    }

    click = ClickWindow; // Clicking anywhere, we use this and override it if we clicked on something specific

    // Check click position and override the value of click
    if (yp && ex < x + ctx.prompt_width + powerlineprompt ? ctx.powerline_width : 0) {
        click = ClickPrompt;
    } else if (yp && (ex > ctx.win_width - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh) && !hidecaps && capsw) {
        click = ClickCaps;
    } else if (yp && ex > ctx.win_width - modew - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh) {
        click = ClickMode;
    } else if (yp && ex > ctx.win_width - modew - numberw - capsw - 2 * ctx.hpadding - 2 * borderwidth - menumarginh) {
        click = ClickNumber;
    } else if (yp && !hideinput) {
        w = (lines > 0 || !matches) ? ctx.win_width - x : ctx.input_width;

        if ((lines <= 0 && ex >= 0 && ex <= x + w + ctx.prompt_width +
                    ((!previousitem || !currentitem->left) ? larroww : 0)) ||
                (lines > 0 && ey >= y && ey <= y + h)) {

            click = ClickInput;
        }
    }

#if IMAGE
    if (!hideimage && img.longest_edge != 0 && imagetype) {
        x += MAX((img.gaps * 2) + img.width, indentitems ? ctx.prompt_width : 0);
    }
#endif

    // It's an item click
    if (lines > 0) {
        w = ctx.win_width - x;

        ey -= menumarginv;

        if ((hideprompt && hideinput && hidemode && hidematchcount && hidecaps) || itemposition) {
            ey += h;
        }

        for (item = currentitem; item != nextitem; item = item->right) {
            if (item_num++ == lines) {
                item_num = 1;
                x += w / columns;
                y = 0;
            }

            y += h;

            if (ey >= y && ey <= (y + h) && ex >= x + (powerlineitems ? ctx.powerline_width : 0) && ex <= (x + w / columns) + (powerlineitems ? ctx.powerline_width : 0)) {
                click = ClickItem;
                mouseitem = item;
#if IMAGE
            } else if (ey >= y && ey <= (y + h) && ex >= x + (powerlineitems ? ctx.powerline_width : 0) - MAX((img.gaps * 2) + img.width, indentitems ? ctx.prompt_width : 0) && ex <= (x - MAX((img.gaps * 2) + img.width, indentitems ? ctx.prompt_width : 0) + w / columns) + (powerlineitems ? ctx.powerline_width : 0)) {
                click = ClickImage;
                mouseitem = item;
#endif
            }
        }
    } else if (matches) { // Single line
        x += ctx.input_width;
        w = larroww;

        if (previousitem && currentitem->left) {
            if (ex >= x && ex <= x + w) {
                click = ClickLArrow;
            }
        }

        w = rarroww;
        x = ctx.win_width - w;

        if (nextitem && ex >= x && ex <= x + w) {
            click = ClickRArrow;
        }
    }

    for (auto& it : wl_buttons) {
        if (ctx.ignore_global_mouse) break;
        if ((click == it.click || it.click == ClickNone) && it.func && it.button == button)
            it.func(it.arg);
    }

    for (auto& it : wl_cbuttons) {
        if (ctx.ignore_conf_mouse) break;
        if ((click == it.click || it.click == ClickNone) && it.func && it.button == button)
            it.func(it.arg);
    }
}

void pointer_motion_handler(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y) {
    mouse_x = wl_fixed_to_int(x);
    mouse_y = wl_fixed_to_int(y);
}

void pointer_axis_handler(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
    if (value > scrolldistance) {
        mouse_scroll_direction = 0;
    } else if (value < -scrolldistance) {
        mouse_scroll_direction = 1;
    } else {
        mouse_scroll = 0;
        mouse_scroll_direction = -1;

        return;
    }

    mouse_scroll = 1;

    buttonpress_wl(mouse_scroll_direction, mouse_x, mouse_y);

    mouse_scroll = 0;
    mouse_scroll_direction = -1;
}

void pointer_button_handler(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    if (state != WL_POINTER_BUTTON_STATE_PRESSED) {
        return; // We don't want a release event to count as a click, only the initial press.
    }

    buttonpress_wl(button, mouse_x, mouse_y);
}

void seat_capabilities(void *data, struct wl_seat *seat, uint32_t caps) {
    struct state* state = static_cast<struct state*>(data);

    if (seat == nullptr) {
        throw std::runtime_error("seat == nullptr");
    }

    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        state->keyboard = wl_seat_get_keyboard(seat);
        state->pointer = wl_seat_get_pointer(seat);
        wl_keyboard_add_listener(state->keyboard, &keyboard_listener, state);
        wl_pointer_add_listener(state->pointer, &pointer_listener, state);
    }
}

void output_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t output_transform) {
    output_physical_width = physical_width;
    output_physical_height = physical_height;
}

void output_mode(void *data, struct wl_output *wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    output_width = width;
    output_height = height;
}

void output_scale(void *data, struct wl_output *wl_output, int32_t factor) {
	struct output* output = static_cast<struct output*>(data);

	output->scale = factor;
}

void output_name(void *data, struct wl_output *wl_output, const char *name) {
	struct output* output = static_cast<struct output*>(data);
	struct state *state = output->state;

	char *outname = state->output_name;

	if (!state->output && outname && strcmp(outname, name) == 0) {
		state->output = output;
	}
}

void layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t width, uint32_t height) {
	zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface) {
    cleanup();
    exit(0);
}

void zero() {
    // Nothing.
}

void create_drawable(struct state *state) {
    if (!allow_draw) { // No drawing if disabled
        return;
    }

    state->buffer = create_buffer(state);

    if (state->buffer == nullptr) {
        die("state->buffer == nullptr");
    }

    draw.initialize(state->data, state->width, state->height);

    draw_menu();
}

void global_handler(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
	struct state* state = static_cast<struct state*>(data);
	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		state->compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name,
				&wl_compositor_interface, 4));
	} else if (strcmp(interface, wl_shm_interface.name) == 0) {
		state->shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1));
	} else if (strcmp(interface, wl_seat_interface.name) == 0 && has_keys) {
		struct wl_seat *seat = static_cast<wl_seat*>(wl_registry_bind(registry, name,
				&wl_seat_interface, 4));
		wl_seat_add_listener(seat, &seat_listener, state);
	} else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
		state->layer_shell = static_cast<zwlr_layer_shell_v1*>(wl_registry_bind(registry, name,
				&zwlr_layer_shell_v1_interface, 1));
	} else if (strcmp(interface, wl_output_interface.name) == 0) {
		struct output *output = static_cast<struct output*>(calloc(1, sizeof(struct output)));
		output->output = static_cast<struct wl_output*>(wl_registry_bind(registry, name,
				&wl_output_interface, 4));
		output->state = state;
		output->scale = 1;

		wl_output_set_user_data(output->output, output);
		wl_output_add_listener(output->output, &output_listener, output);
	}
}

void surface_enter(void *data, struct wl_surface *surface, struct wl_output *wl_output) {
    struct state *state = static_cast<struct state*>(data);
    state->output = static_cast<output*>(wl_output_get_user_data(wl_output));

    match();
    create_drawable(state);
}

void commit_drawable(struct state *state) {
    wl_surface_set_buffer_scale(state->surface, 1);
    wl_surface_attach(state->surface, state->buffer, 0, 0);
    wl_surface_damage(state->surface, 0, 0, state->width, state->height);
    wl_surface_commit(state->surface);
}

int roundtrip(struct state *state) {
    if (wl_display_roundtrip(state->display)) {
        return 0;
    } else {
        return 1;
    }
}

int connect_display(struct state *state) {
    state->display = wl_display_connect(nullptr);

    if (state->display) {
        return 0;
    } else {
        return 1;
    }
}

int disconnect_display(struct state *state) {
    wl_display_disconnect(state->display);

    return 0;
}

int await_dispatch(struct state *state) {
    struct pollfd fds[] = {
        { wl_display_get_fd(state->display), POLLIN, 0 },
		{ state->timer, POLLIN, 0 },
	};

    const int nfds = sizeof(fds) / sizeof(*fds);

    for (;;) {
        errno = 0;

        do {
			if (wl_display_flush(state->display) == -1 && errno != EAGAIN) {
				fprintf(stderr, "majorna: wl_display_flush failed: %s\n", strerror(errno));
				break;
			}
		} while (errno == EAGAIN);

        if (poll(fds, nfds, -1) < 0) {
			fprintf(stderr, "majorna: poll failed: %s\n", strerror(errno));
			break;
		}

		if (fds[0].revents & POLLIN) {
			if (wl_display_dispatch(state->display) < 0) {
                break;
			}
		}

		if (fds[1].revents & POLLIN) {
			keyboard_repeat(state);
		}
    }

    return 0;
}

/* If this function returns 1, something went wrong.
 * This may be that the user is using X11, or a compositor like Mutter.
 * In this case, it may be a good idea to fall back to X11.
 */
int init_disp(struct state *state) {
    if (!state->display) {
        return 1;
    } else {
        no_display = 0;
    }

    struct wl_registry *registry = wl_display_get_registry(state->display);
    wl_registry_add_listener(registry, &registry_listener, state);
    wl_display_roundtrip(state->display);

    if (state->compositor == nullptr) {
        die("majorna: Your compositor does not implement the wl-compositor protocol. Run majorna in X11 mode.");
    }

    if (state->layer_shell == nullptr) {
        die("majorna: Your compositor does not implement the wlr-layer-shell protocol. Run majorna in X11 mode.");
    }

    if (state->shm == nullptr) {
        die("majorna: Your compositor does not implement the wl-shm protocol. Run majorna in X11 mode.");
    }

    wl_display_roundtrip(state->display);

    if (state->output_name && !state->output) {
        return 1;
    }

    return 0;
}

void resizeclient_wl(struct state *state) {
    item *item;
    int mh = ctx.win_height;
    int ic = 0;

    for (item = items; item && item->raw_text; item++)
        ic++;

    lines = MAX(MIN(ic, MAX(lines, 0)), minlines);
#if IMAGE
    img.set_lines = lines;
#endif
    get_mh();

    if (hideprompt && hideinput && hidemode && hidematchcount && hidecaps) {
        ctx.win_height -= ctx.item_height;
    }

    if (ctx.win_height == mh) {
        return;
    }

    state->width = ctx.win_width;
    state->height = ctx.win_height;

    state->buffer = create_buffer(state);

    if (state->buffer == nullptr) {
        die("state->buffer == null");
    }

    draw.initialize(state->data, state->width, state->height);

    set_layer_size(state, state->width, state->height);
    commit_drawable(state);
}

/* It is advised you call this function right before calling init_disp()
 * It sets up keybinds for you.
 */
int init_keys(struct state *state) {
    state->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    if (!state->xkb_context) {
        return 1;
    } else {
        has_keys = 1;
    }

    state->timer = timerfd_create(CLOCK_MONOTONIC, 0);

    if (state->timer < 0) {
        has_keys = 0;
        fprintf(stderr, "timerfd_create() failed\n");

        return 1;
    }

    return 0;
}

int create_layer(struct state *state, char *name) {
    if (state->compositor == nullptr) {
        throw std::runtime_error("state->compositor == nullptr");
    }

    state->surface = wl_compositor_create_surface(state->compositor);

    if (!state->layer_shell) {
        throw std::runtime_error("state->layer_shell == nullptr");
    }
    if (!state->surface) {
        throw std::runtime_error("state->surface == nullptr");
    }

    wl_surface_add_listener(state->surface, &surface_listener, state);
    state->layer_surface = zwlr_layer_shell_v1_get_layer_surface(
            state->layer_shell,
            state->surface,
            nullptr,
            ZWLR_LAYER_SHELL_V1_LAYER_TOP,
            name);

    if (!state->layer_surface) {
        throw std::runtime_error("state->layer_surface == nullptr");
    }

    return 0;
}

int anchor_layer(struct state *state, int position) {
    if (state->layer_surface == nullptr) {
        throw std::runtime_error("state->layer_surface == nullptr");
    }
    zwlr_layer_surface_v1_set_anchor(
            state->layer_surface,
            ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
            ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT |
            (position == 0 ? ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM :
             position == 1 ? ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP :
             0));

    return 0;
}

int set_layer_size(struct state *state, int32_t width, int32_t height) {
    if (state->layer_surface == nullptr) {
        throw std::runtime_error("state->layer_surface == nullptr");
    }

    zwlr_layer_surface_v1_set_size(state->layer_surface, width, height);

    return 0;
}

int set_exclusive_zone(struct state *state, unsigned int val) {
    if (state->layer_surface == nullptr) {
        throw std::runtime_error("state->layer_surface == nullptr");
    }

    zwlr_layer_surface_v1_set_exclusive_zone(state->layer_surface, val);

    return 0;
}

int set_keyboard(struct state *state, int interactivity) {
    if (state->layer_surface == nullptr) {
        throw std::runtime_error("state->layer_surface == nullptr");
    }

    zwlr_layer_surface_v1_set_keyboard_interactivity(state->layer_surface, interactivity ? true : false);

    return 0;
}

int add_layer_listener(struct state *state) {
    if (!state->layer_surface) {
        throw std::runtime_error("state->layer_surface == nullptr");
    }

    zwlr_layer_surface_v1_add_listener(state->layer_surface, &layer_surface_listener, state);

    return 0;
}

int set_visible_layer(struct state *state) {
    if (!state->layer_surface) {
        throw std::runtime_error("state->layer_surface == nullptr");
    }
    if (!state->surface) {
        throw std::runtime_error("state->surface == nullptr");
    }

    wl_surface_commit(state->surface);

    return 0;
}
