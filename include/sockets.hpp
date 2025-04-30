#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <arg.hpp>
#include <draw.hpp>
#include <img.hpp>
#include <match.hpp>
#include <majorna.hpp>
#include <stream.hpp>
#include <nlohmann/json.hpp>

static inline std::unordered_map<std::string, std::function<void(const Arg& arg)>> arg_map {
    {"move_up", move_up},
    {"move_down", move_down},
    {"move_left", move_left},
    {"move_right", move_right},
    {"move_end", move_end},
    {"move_start", move_start},
    {"move_next", move_next},
    {"move_prev", move_prev},
    {"move_item", move_item},
    {"move_word", move_word},
    {"move_cursor", move_cursor},
    {"paste", paste},
    {"restore_selection", restore_selection},
    {"clear", clear},
    {"clear_and_insert", clear_and_insert},
    {"delete_word", delete_word},
    {"backspace", backspace},
    {"complete", complete},
    {"view_history", view_history},
    {"navigate_history", navigate_history},
    {"mark_item", mark_item},
    {"select_item", select_item},
    {"set_image_size", set_image_size},
    {"reset_image", reset_image},
    {"flip_image", flip_image},
    {"set_image_position", set_image_position},
    {"set_image_gaps", set_image_gaps},
    {"set_lines", set_lines},
    {"set_columns", set_columns},
    {"spawn", spawn},
    {"screenshot", screenshot},
    {"toggle_mode", toggle_mode},
    {"set_profile", set_profile},
    {"set_line_height", set_line_height},
    {"set_x", set_x},
    {"set_y", set_y},
    {"set_w", set_w},
    {"quit", quit},
    {"toggle_input", toggle_input},
    {"toggle_pretext", toggle_pretext},
    {"toggle_left_arrow", toggle_left_arrow},
    {"toggle_right_arrow", toggle_right_arrow},
    {"toggle_item", toggle_item},
    {"toggle_prompt", toggle_prompt},
    {"toggle_caps", toggle_caps},
    {"toggle_powerline", toggle_powerline},
    {"toggle_caret", toggle_caret},
    {"toggle_match_count", toggle_match_count},
    {"toggle_mode_indicator", toggle_mode_indicator},
    {"toggle_highlight", toggle_highlight},
    {"toggle_regex", toggle_regex},
    {"toggle_fuzzy", toggle_fuzzy_matching},
    {"toggle_image", toggle_image},
    {"toggle_image_type", toggle_image_type},
    {"select_hover", select_hover},
    {"output_hover", output_hover},
    {"mark_hover", mark_hover},
};

static inline std::unordered_map<std::string, std::function<void()>> void_map {
    {"draw_menu", draw_menu},
    {"draw_menu_layer", draw_menu_layer},
    {"draw_image", draw_image},
    {"resize_client", resize_client},
    {"match", match},
    {"read_stdin", read_stdin},
};

void initialize_socket();
void load_config(nlohmann::json&);
std::string handler(const std::string&);
