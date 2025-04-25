#pragma once

#include <string>

/* structure which may be used to represent any argument type
 * we'll be using them to pass arguments to functions
 */
struct Arg {
    int i{};
    unsigned int ui{};
    float f{};
    const void *v{};
    std::string c{};
};

/* declare keybind functions
 *
 * movement
 */
void move_up(const Arg& arg);
void move_down(const Arg& arg);
void move_left(const Arg& arg);
void move_right(const Arg& arg);
void move_end(const Arg& arg);
void move_start(const Arg& arg);
void move_next(const Arg& arg);
void move_prev(const Arg& arg);
void move_item(const Arg& arg);
void move_word(const Arg& arg);
void move_cursor(const Arg& arg);

/* input */
void paste(const Arg& arg);
void restore_selection(const Arg& arg);
void clear(const Arg& arg);
void clear_and_insert(const Arg& arg);
void delete_word(const Arg& arg);
void backspace(const Arg& arg);
void complete(const Arg& arg);

/* history */
void view_history(const Arg& arg);
void navigate_history(const Arg& arg);

/* selection */
void mark_item(const Arg& arg);
void select_item(const Arg& arg);

/* image */
void set_image_size(const Arg& arg);
void reset_image(const Arg& arg);
void flip_image(const Arg& arg);
void set_image_position(const Arg& arg);
void set_image_gaps(const Arg& arg);

/* lines/columns */
void set_lines(const Arg& arg);
void set_columns(const Arg& arg);

/* misc */
void spawn(const Arg& arg);
void screenshot(const Arg& arg);
void toggle_mode(const Arg& arg);
void set_profile(const Arg& arg);
void set_line_height(const Arg& arg);
void set_x(const Arg& arg);
void set_y(const Arg& arg);
void set_w(const Arg& arg);
void quit(const Arg& arg);

/* toggle */
void toggle_input(const Arg& arg);
void toggle_pretext(const Arg& arg);
void toggle_left_arrow(const Arg& arg);
void toggle_right_arrow(const Arg& arg);
void toggle_item(const Arg& arg);
void toggle_prompt(const Arg& arg);
void toggle_caps(const Arg& arg);
void toggle_powerline(const Arg& arg);
void toggle_caret(const Arg& arg);
void toggle_match_count(const Arg& arg);
void toggle_mode_indicator(const Arg& arg);
void toggle_highlight(const Arg& arg);
void toggle_regex(const Arg& arg);
void toggle_fuzzy_matching(const Arg& arg);
void toggle_image(const Arg& arg);
void toggle_image_type(const Arg& arg);

/* hover */
void select_hover(const Arg& arg);
void output_hover(const Arg& arg);
void mark_hover(const Arg& arg);
