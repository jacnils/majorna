#include <wl/init.hpp>
#include <majorna.hpp>
#include <options.hpp>
#include <match.hpp>
#include <macros.hpp>
#include <clocale>

void prepare_window_size_wl() {
    ctx.hpadding = menupaddingh;
    ctx.vpadding = (menuposition == 1) ? menupaddingv : - menupaddingv;

    ctx.item_height = std::max(draw.get_font_manager().get_height(), draw.get_font_manager().get_height() + 2 + lineheight);
    lines = MAX(lines, 0);
#if IMAGE
    img.set_lines = lines;
#endif

    ctx.lrpad = draw.get_font_manager().get_height() + textpadding;
}

void handle_wl() {
    if (!setlocale(LC_CTYPE, "")) {
        die("majorna: no locale support");
    }

    prepare_window_size_wl();
    ctx.prompt_width = !prompt.empty() ? pango_prompt ? TEXTWM(prompt) : TEXTW(prompt) - ctx.lrpad / 4 : 0;

    allow_draw = 1;

    init_keys(&state);
    init_disp(&state);

    if (no_display) {
        die("majorna: failed to connect to wayland display");
    }

    create_layer(&state, "majorna");

    monitor.output_width = output_width;
    monitor.output_height = output_height;

    ctx.win_width = (menuwidth > 0 ? menuwidth : monitor.output_width);
    get_mh();

    if (menuposition == 2) {
        ctx.win_width = MIN(MAX(max_textw() + ctx.prompt_width, centerwidth), monitor.output_width);
    }

    state.width = ctx.win_width;
    state.height = ctx.win_height;

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
