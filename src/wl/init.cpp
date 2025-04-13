#include <wl/init.hpp>
#include <majorna.hpp>
#include <options.hpp>
#include <match.hpp>
#include <macros.hpp>
#include <locale.h>

void prepare_window_size_wl() {
    sp.sp = menupaddingh;
    sp.vp = (menuposition == 1) ? menupaddingv : - menupaddingv;

    sp.bh = std::max(draw.get_font_manager().get_height(), draw.get_font_manager().get_height() + 2 + lineheight);
    lines = MAX(lines, 0);
#if IMAGE
    img.setlines = lines;
#endif

    sp.lrpad = draw.get_font_manager().get_height() + textpadding;
}

void handle_wl(void) {
    if (!setlocale(LC_CTYPE, "")) {
        die("majorna: no locale support");
    }

    prepare_window_size_wl();
    sp.promptw = (prompt && *prompt) ? pango_prompt ? TEXTWM(prompt) : TEXTW(prompt) - sp.lrpad / 4 : 0;

    allow_draw = 1;

    init_keys(&state);
    init_disp(&state);

    if (no_display) {
        die("majorna: failed to connect to wayland display");
    }

    create_layer(&state, "majorna");

    mo.output_width = output_width;
    mo.output_height = output_height;

    sp.mw = (menuwidth > 0 ? menuwidth : mo.output_width);
    get_mh();

    if (menuposition == 2) {
        sp.mw = MIN(MAX(max_textw() + sp.promptw, centerwidth), mo.output_width);
    }

    state.width = sp.mw;
    state.height = sp.mh;

    set_layer_size(&state, state.width, state.height);

    anchor_layer(&state, menuposition);
    set_exclusive_zone(&state, -1);
    set_keyboard(&state, grabkeyboard ? 1 : 0);
    add_layer_listener(&state);
    set_visible_layer(&state);

    roundtrip(&state);

    match();
    create_drawable(&state);

    await_dispatch(&state);

    disconnect_display(&state);
}
