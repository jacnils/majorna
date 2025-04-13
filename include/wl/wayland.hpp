#pragma once

#if WAYLAND

#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <xkbcommon/xkbcommon.h>
#include "wlr-layer-shell-unstable-v1-client-protocol.h"

struct output {
    struct state *state;
    struct wl_output *output;

    int32_t scale;
};

typedef struct {
    unsigned int mode;
    char *modifier;
    xkb_keysym_t keysym;
    void (*func)(Arg *);
    Arg arg;
} WlKey;

typedef struct {
    unsigned int click;
    unsigned int button;
    void (*func)(Arg *arg);
    Arg arg;
} WlMouse;

inline WlKey wl_ckeys[256];
inline WlMouse wl_cbuttons[256];

#define WL_CtrlShift "CtrlShift"
#define WL_CtrlShiftSuper "CtrlShiftSuper"
#define WL_CtrlSuper "CtrlSuper"
#define WL_CtrlAlt "CtrlAlt"
#define WL_CtrlAltShift "CtrlAltShift"
#define WL_CtrlAltShiftSuper "CtrlAltShiftSuper"
#define WL_CtrlAltSuper "CtrlAltSuper"
#define WL_AltShift "AltShift"
#define WL_Shift "Shift"
#define WL_Ctrl "Ctrl"
#define WL_Alt "Alt"
#define WL_Super "Super"
#define WL_None "None"

#define WL_Left	0x110
#define WL_Right 0x111
#define WL_Middle 0x112
#define WL_Side	0x113
#define WL_Extra 0x114
#define WL_Forward 0x115
#define WL_Back	0x116
#define WL_Task	0x117
#define WL_Up 1
#define WL_Down 0

inline WlKey wlhkeys[1] = { { static_cast<unsigned int>(-1), WL_CtrlAlt, XKB_KEY_Delete, quit, {0} } };

struct state {
    struct output *output;
	char *output_name;

	struct wl_display *display;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
    struct wl_seat *seat;
	struct wl_keyboard *keyboard;
    struct wl_pointer *pointer;
	struct wl_surface *surface;

	struct zwlr_layer_shell_v1 *layer_shell;
	struct zwlr_layer_surface_v1 *layer_surface;

	int width;
	int height;

    /* Taken from wmenu */
    int timer;
    int delay;
    int period;

    struct xkb_context *xkb_context;
	struct xkb_keymap *xkb_keymap;
	struct xkb_state *xkb_state;

    enum wl_keyboard_key_state repeat_key_state;

    xkb_keysym_t repeat_sym;

    void *data;
    struct wl_buffer *buffer;
};

inline state state = { 0 };

inline int output_physical_width = 0;
inline int output_physical_height = 0;
inline int output_width = 0;
inline int output_height = 0;
inline int mouse_x = 0;
inline int mouse_y = 0;
inline int mouse_scroll = 0;
inline int mouse_scroll_direction = 0;

void resizeclient_wl(struct state *state);
void output_scale(void *data, struct wl_output *wl_output, int32_t factor);
void output_name(void *data, struct wl_output *wl_output, const char *name);
void output_geometry(void *data, struct wl_output *wl_output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t output_transform);
void output_mode(void *data, struct wl_output *wl_output, uint32_t flags, int32_t width, int32_t height, int32_t refresh);
void layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface, uint32_t serial, uint32_t width, uint32_t height);
void layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface);
void create_drawable(struct state *state);
void commit_drawable(struct state *state);
void global_handler(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
//void seat_capabilities(void *data, struct wl_seat *seat, enum wl_seat_capability caps);
void seat_capabilities(void *data, struct wl_seat *seat, uint32_t caps);
void surface_enter(void *data, struct wl_surface *surface, struct wl_output *wl_output);
void keyboard_keymap(void *data, struct wl_keyboard *wl_keyboard, uint32_t format, int32_t fd, uint32_t size);
void keyboard_key(void *data, struct wl_keyboard *wl_keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t _key_state);
void keyboard_modifiers(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
void keyboard_repeat(struct state *state);
void keyboard_repeat_info(void *data, struct wl_keyboard *wl_keyboard, int32_t rate, int32_t delay);
void keypress_wl(struct state *state, enum wl_keyboard_key_state key_state, xkb_keysym_t sym);
void pointer_button_handler(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
void pointer_motion_handler(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
void pointer_axis_handler(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
void buttonpress_wl(uint32_t button, double ex, double ey);
int is_correct_modifier(struct state *state, char *modifier);
int roundtrip(struct state *state);
int init_disp(struct state *state);
int connect_display(struct state *state);
int disconnect_display(struct state *state);
int await_dispatch(struct state *state);
int init_keys(struct state *state);
int create_layer(struct state *state, char *name);
int anchor_layer(struct state *state, int position);
int set_layer_size(struct state *state, int32_t width, int32_t height);
int set_exclusive_zone(struct state *state, unsigned int val);
int set_keyboard(struct state *state, int interactivity);
int add_layer_listener(struct state *state);
int set_visible_layer(struct state *state);
char *wl_clipboard();
void paste_wl();

wl_buffer *create_buffer(struct state *state);

/* Set to 0 if the connection was successful
 * You can use this for say, X11 compatibility mode.
 */
inline int no_display = 1;

/* Note that you MUST override this before calling create_layer() because
 * otherwise nothing will be drawn. Do not forget to do this.
 */
inline int allow_draw = 0;

/* Set automatically */
inline int has_keys = 0;

/* See global_handler */
inline const struct wl_registry_listener registry_listener = {
    .global = global_handler,
    .global_remove = [](void *, struct wl_registry*, uint32_t) {},
};

/* See surface_enter */
inline const struct wl_surface_listener surface_listener = {
    .enter = surface_enter,
    .leave = [](void *, struct wl_surface*, struct wl_output*) {},
};

inline const struct wl_keyboard_listener keyboard_listener = {
	.keymap = keyboard_keymap,
	.enter = [](void *, struct wl_keyboard*, uint32_t, wl_surface*, struct wl_array*) {},
	.leave = [](void *, struct wl_keyboard*, uint32_t, wl_surface*) {},
	.key = keyboard_key,
	.modifiers = keyboard_modifiers,
	.repeat_info = keyboard_repeat_info,
};

inline const struct wl_pointer_listener pointer_listener = {
    .enter = [](void *, struct wl_pointer*, uint32_t, struct wl_surface*, wl_fixed_t, wl_fixed_t) {},
    .leave = [](void *, struct wl_pointer*, uint32_t, struct wl_surface*) {},
    .motion = pointer_motion_handler,
    .button = pointer_button_handler,
    .axis = pointer_axis_handler,
};

inline struct wl_output_listener output_listener = {
    .geometry = output_geometry,
    .mode = output_mode,
    .done = [](void *, struct wl_output*) {},
    .scale = output_scale,
    .name = output_name,
    .description = [](void *, struct wl_output*, const char *) {},
};

inline struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_configure,
    .closed = layer_surface_closed,
};

inline struct wl_seat_listener seat_listener = {
    .capabilities = seat_capabilities,
    .name = [](void *, struct wl_seat*, const char *) {},
};

inline const struct wl_buffer_listener buffer_listener = {
    .release = [](void *data, struct wl_buffer *buffer) {},
};

#endif
