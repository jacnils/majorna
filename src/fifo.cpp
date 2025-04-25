/* See LICENSE file for copyright and license details. */

#include <iostream>
#include <fifo.hpp>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <options.hpp>
#include <config/config.hpp>
#include <unistd.h>
#include <majorna.hpp>
#include <draw.hpp>
#include <match.hpp>

void execute_fifo_cmd() {
    int fd;
    char _fifot[BUFSIZ];

    std::cerr << "Warning: FIFO is deprecated and will be removed in a future release. Use the Majorna sockets API instead.\n";

    done = 0;

    fd = open(fifofile.c_str(), O_RDONLY);
    int r = read(fd, _fifot, sizeof(_fifot));

    if (!r) {
        close(fd);
        done = 1;

        return;
    }

    std::string fifot = _fifot;

    // remove newlines
    fifot.erase(std::remove_if(fifot.begin(), fifot.end(), [](char c) {
        return c == '\n';
    }));

    /* These are the different commands that we can run
     * by outputting text to the FIFO.
     */
    if (fifot == "drawmenu") {
        if (!ctx.is_drawing) {
            draw_menu();
        }
    } else if (fifot == "match") {
        match();
    } else if (fifot == "update") {
        if (!ctx.is_drawing) {
            match();
            draw_menu();
        }
    } else if (fifot == "test") {
        fprintf(stderr, "Test print\n");
    } else if (fifot == "die") {
        close(fd);
        die("FIFO told me to die.");
    } else if (fifot == "loadconfig") {
#if CONFIG
        conf_init();
#endif
    } else if (fifot == "output") {
        if (selecteditem) {
            for (int i = 0; i < sel_size; i++) {
                if (sel_index[i] != -1 && (!selecteditem || selecteditem->index != sel_index[i])) {
                    puts(items[sel_index[i]].raw_text);
                }
            }
        }
    } else if (fifot == "output_index") {
        if (printindex && selecteditem) {
            fprintf(stdout, "%d\n", selecteditem->index);
        }
    } else if (fifot == "toggle_input") {
        Arg arg;
        toggle_input(arg);
    } else if (fifot == "toggle_left_arrow") {
        Arg arg;
        toggle_left_arrow(arg);
    } else if (fifot == "toggle_right_arrow") {
        Arg arg;
        toggle_right_arrow(arg);
    } else if (fifot == "toggle_item") {
        Arg arg;
        toggle_item(arg);
    } else if (fifot == "toggle_prompt") {
        Arg arg;
        toggle_prompt(arg);
    } else if (fifot == "toggle_caps") {
        Arg arg;
        toggle_caps(arg);
    } else if (fifot == "toggle_powerline") {
        Arg arg;
        toggle_powerline(arg);
    } else if (fifot == "toggle_caret") {
        Arg arg;
        toggle_caret(arg);
    } else if (fifot == "toggle_highlight") {
        Arg arg;
        toggle_highlight(arg);
    } else if (fifot == "toggle_match_count") {
        Arg arg;
        toggle_match_count(arg);
    } else if (fifot == "toggle_mode_indicator") {
        Arg arg;
        toggle_mode_indicator(arg);
    } else if (fifot == "toggle_regex") {
        Arg arg;
        toggle_regex(arg);
    } else if (fifot == "toggle_fuzzy_matching") {
        Arg arg;
        toggle_fuzzy_matching(arg);
    } else if (fifot == "toggle_image") {
        Arg arg;
        toggle_image(arg);
    } else if (fifot == "toggle_image_type") {
        Arg arg;
        toggle_image_type(arg);
    } else if (fifot == "screenshot") {
        Arg arg;
        screenshot(arg);
    } else if (fifot == "set_profile") {
        Arg arg;
        set_profile(arg);
    } else if (fifot == "set_lines+") {
        Arg arg;
        arg.i = 1;
        set_lines(arg);
    } else if (fifot == "set_lines-") {
        Arg arg;
        arg.i = -1;
        set_lines(arg);
    } else if (fifot == "set_columns+") {
        Arg arg;
        arg.i = 1;
        set_columns(arg);
    } else if (fifot == "set_columns-") {
        Arg arg;
        arg.i = -1;
        set_columns(arg);
    } else if (fifot == "set_x+") {
        Arg arg;
        arg.i = 1;
        set_x(arg);
    } else if (fifot == "set_x-") {
        Arg arg;
        arg.i = -1;
        set_x(arg);
    } else if (fifot == "set_y+") {
        Arg arg;
        arg.i = 1;
        set_y(arg);
    } else if (fifot == "set_y-") {
        Arg arg;
        arg.i = -1;
        set_y(arg);
    } else if (fifot == "set_w+") {
        Arg arg;
        arg.i = 1;
        set_w(arg);
    } else if (fifot == "set_w-") {
        Arg arg;
        arg.i = -1;
        set_w(arg);
    } else if (fifot == "move_up") {
        Arg arg;
        move_up(arg);
    } else if (fifot == "move_down") {
        Arg arg;
        move_down(arg);
    } else if (fifot == "move_left") {
        Arg arg;
        move_left(arg);
    } else if (fifot == "move_right") {
        Arg arg;
        move_right(arg);
    } else if (fifot == "move_start") {
        Arg arg;
        move_start(arg);
    } else if (fifot == "move_end") {
        Arg arg;
        move_end(arg);
    } else if (fifot == "move_next") {
        Arg arg;
        move_next(arg);
    } else if (fifot == "move_prev") {
        Arg arg;
        move_prev(arg);
    } else if (fifot == "view_history") {
        Arg arg;
        view_history(arg);
    } else if (fifot == "backspace") {
        Arg arg;
        backspace(arg);
    } else if (fifot == "delete_word") {
        Arg arg;
        delete_word(arg);
    } else if (fifot == "move_word+") {
        Arg arg;
        arg.i = +1;
        move_word(arg);
    } else if (fifot == "move_word-") {
        Arg arg;
        arg.i = -1;
        move_word(arg);
    } else if (fifot == "move_cursor+") {
        Arg arg;
        arg.i = +1;
        move_cursor(arg);
    } else if (fifot == "move_cursor-") {
        Arg arg;
        arg.i = -1;
        move_cursor(arg);
    } else if (fifot == "clear") {
        Arg arg;
        clear(arg);
    } else if (fifot == "exit_0") {
        exit(0);
    } else if (fifot == "exit_1") {
        exit(1);
    } else {
        std::cerr << "majorna: Unknown command: " << fifot << "\n";
    }

    close(fd);

    mkfifo(fifofile.c_str(), 0660);

    done = 1;
}

void *fifocmd(void *n) {
    for (;;) {
        if (done) {
            execute_fifo_cmd();
        }

        msleep(static_cast<float>(0.1));
    }
}

void init_fifo() {
    mkfifo(fifofile.c_str(), 0660);
    pthread_t tid;
    pthread_create(&tid, NULL, &fifocmd, NULL);
}
