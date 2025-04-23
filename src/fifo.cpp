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
            drawmenu();
        }
    } else if (fifot == "match") {
        match();
    } else if (fifot == "update") {
        if (!ctx.is_drawing) {
            match();
            drawmenu();
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
    } else if (fifot == "toggleinput") {
        Arg arg;
        toggleinput(arg);
    } else if (fifot == "togglelarrow") {
        Arg arg;
        togglelarrow(arg);
    } else if (fifot == "togglerarrow") {
        Arg arg;
        togglerarrow(arg);
    } else if (fifot == "toggleitem") {
        Arg arg;
        toggleitem(arg);
    } else if (fifot == "toggleprompt") {
        Arg arg;
        toggleprompt(arg);
    } else if (fifot == "togglecaps") {
        Arg arg;
        togglecaps(arg);
    } else if (fifot == "togglepowerline") {
        Arg arg;
        togglepowerline(arg);
    } else if (fifot == "togglecaret") {
        Arg arg;
        togglecaret(arg);
    } else if (fifot == "togglehighlight") {
        Arg arg;
        togglehighlight(arg);
    } else if (fifot == "togglematchcount") {
        Arg arg;
        togglematchcount(arg);
    } else if (fifot == "togglemode") {
        Arg arg;
        togglemode(arg);
    } else if (fifot == "toggleregex") {
        Arg arg;
        toggleregex(arg);
    } else if (fifot == "togglefuzzy") {
        Arg arg;
        togglefuzzy(arg);
    } else if (fifot == "toggleimg") {
        Arg arg;
        toggleimg(arg);
    } else if (fifot == "toggleimgtype") {
        Arg arg;
        toggleimgtype(arg);
    } else if (fifot == "screenshot") {
        Arg arg;
        screenshot(arg);
    } else if (fifot == "setprofile") {
        Arg arg;
        setprofile(arg);
    } else if (fifot == "setlines+") {
        Arg arg;
        arg.i = 1;
        setlines(arg);
    } else if (fifot == "setlines-") {
        Arg arg;
        arg.i = -1;
        setlines(arg);
    } else if (fifot == "setcolumns+") {
        Arg arg;
        arg.i = 1;
        setcolumns(arg);
    } else if (fifot == "setcolumns-") {
        Arg arg;
        arg.i = -1;
        setcolumns(arg);
    } else if (fifot == "setx+") {
        Arg arg;
        arg.i = 1;
        setx(arg);
    } else if (fifot == "setx-") {
        Arg arg;
        arg.i = -1;
        setx(arg);
    } else if (fifot == "sety+") {
        Arg arg;
        arg.i = 1;
        sety(arg);
    } else if (fifot == "sety-") {
        Arg arg;
        arg.i = -1;
        sety(arg);
    } else if (fifot == "setw+") {
        Arg arg;
        arg.i = 1;
        setw(arg);
    } else if (fifot == "setw-") {
        Arg arg;
        arg.i = -1;
        setw(arg);
    } else if (fifot == "moveup") {
        Arg arg;
        moveup(arg);
    } else if (fifot == "movedown") {
        Arg arg;
        movedown(arg);
    } else if (fifot == "moveleft") {
        Arg arg;
        moveleft(arg);
    } else if (fifot == "moveright") {
        Arg arg;
        moveright(arg);
    } else if (fifot == "movestart") {
        Arg arg;
        movestart(arg);
    } else if (fifot == "moveend") {
        Arg arg;
        moveend(arg);
    } else if (fifot == "movenext") {
        Arg arg;
        movenext(arg);
    } else if (fifot == "moveprev") {
        Arg arg;
        moveprev(arg);
    } else if (fifot == "viewhist") {
        Arg arg;
        viewhist(arg);
    } else if (fifot == "backspace") {
        Arg arg;
        backspace(arg);
    } else if (fifot == "deleteword") {
        Arg arg;
        deleteword(arg);
    } else if (fifot == "moveword+") {
        Arg arg;
        arg.i = +1;
        moveword(arg);
    } else if (fifot == "moveword-") {
        Arg arg;
        arg.i = -1;
        moveword(arg);
    } else if (fifot == "movecursor+") {
        Arg arg;
        arg.i = +1;
        movecursor(arg);
    } else if (fifot == "movecursor-") {
        Arg arg;
        arg.i = -1;
        movecursor(arg);
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
