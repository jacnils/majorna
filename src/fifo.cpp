/* See LICENSE file for copyright and license details. */

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
#include <filesystem>

void execute_fifo_cmd() {
    int fd;
    char fifot[BUFSIZ];

    done = 0;

    fd = open(fifofile.c_str(), O_RDONLY);
    int r = read(fd, fifot, sizeof(fifot));

    if (!r) {
        close(fd);
        done = 1;

        return;
    }

    /* These are the different commands that we can run
     * by outputting text to the FIFO.
     */
    if (!strcmp(fifot, "drawmenu")) {
        if (!ctx.is_drawing) {
            drawmenu();
        }
    } else if (!strcmp(fifot, "match")) {
        match();
    } else if (!strcmp(fifot, "update")) {
        if (!ctx.is_drawing) {
            match();
            drawmenu();
        }
    } else if (!strcmp(fifot, "test")) {
        fprintf(stderr, "Test print\n");
    } else if (!strcmp(fifot, "die")) {
        close(fd);
        die("FIFO told me to die.");
    } else if (!strcmp(fifot, "loadconfig")) {
#if CONFIG
        conf_init();
#endif
    } else if (!strcmp(fifot, "output")) {
        if (selecteditem) {
            for (int i = 0; i < sel_size; i++) {
                if (sel_index[i] != -1 && (!selecteditem || selecteditem->index != sel_index[i])) {
                    puts(items[sel_index[i]].raw_text);
                }
            }
        }
    } else if (!strcmp(fifot, "output_index")) {
        if (printindex && selecteditem) {
            fprintf(stdout, "%d\n", selecteditem->index);
        }
    } else if (!strcmp(fifot, "toggleinput")) {
        Arg arg;
        toggleinput(arg);
    } else if (!strcmp(fifot, "togglelarrow")) {
        Arg arg;
        togglelarrow(arg);
    } else if (!strcmp(fifot, "togglerarrow")) {
        Arg arg;
        togglerarrow(arg);
    } else if (!strcmp(fifot, "toggleitem")) {
        Arg arg;
        toggleitem(arg);
    } else if (!strcmp(fifot, "toggleprompt")) {
        Arg arg;
        toggleprompt(arg);
    } else if (!strcmp(fifot, "togglecaps")) {
        Arg arg;
        togglecaps(arg);
    } else if (!strcmp(fifot, "togglepowerline")) {
        Arg arg;
        togglepowerline(arg);
    } else if (!strcmp(fifot, "togglecaret")) {
        Arg arg;
        togglecaret(arg);
    } else if (!strcmp(fifot, "togglehighlight")) {
        Arg arg;
        togglehighlight(arg);
    } else if (!strcmp(fifot, "togglematchcount")) {
        Arg arg;
        togglematchcount(arg);
    } else if (!strcmp(fifot, "togglemode")) {
        Arg arg;
        togglemode(arg);
    } else if (!strcmp(fifot, "toggleregex")) {
        Arg arg;
        toggleregex(arg);
    } else if (!strcmp(fifot, "togglefuzzy")) {
        Arg arg;
        togglefuzzy(arg);
    } else if (!strcmp(fifot, "toggleimg")) {
        Arg arg;
        toggleimg(arg);
    } else if (!strcmp(fifot, "toggleimgtype")) {
        Arg arg;
        toggleimgtype(arg);
    } else if (!strcmp(fifot, "screenshot")) {
        Arg arg;
        screenshot(arg);
    } else if (!strcmp(fifot, "setprofile")) {
        Arg arg;
        setprofile(arg);
    } else if (!strcmp(fifot, "setlines+")) {
        Arg arg;
        arg.i = 1;
        setlines(arg);
    } else if (!strcmp(fifot, "setlines-")) {
        Arg arg;
        arg.i = -1;
        setlines(arg);
    } else if (!strcmp(fifot, "setcolumns+")) {
        Arg arg;
        arg.i = 1;
        setcolumns(arg);
    } else if (!strcmp(fifot, "setcolumns-")) {
        Arg arg;
        arg.i = -1;
        setcolumns(arg);
    } else if (!strcmp(fifot, "setx+")) {
        Arg arg;
        arg.i = 1;
        setx(arg);
    } else if (!strcmp(fifot, "setx-")) {
        Arg arg;
        arg.i = -1;
        setx(arg);
    } else if (!strcmp(fifot, "sety+")) {
        Arg arg;
        arg.i = 1;
        sety(arg);
    } else if (!strcmp(fifot, "sety-")) {
        Arg arg;
        arg.i = -1;
        sety(arg);
    } else if (!strcmp(fifot, "setw+")) {
        Arg arg;
        arg.i = 1;
        setw(arg);
    } else if (!strcmp(fifot, "setw-")) {
        Arg arg;
        arg.i = -1;
        setw(arg);
    } else if (!strcmp(fifot, "moveup")) {
        Arg arg;
        moveup(arg);
    } else if (!strcmp(fifot, "movedown")) {
        Arg arg;
        movedown(arg);
    } else if (!strcmp(fifot, "moveleft")) {
        Arg arg;
        moveleft(arg);
    } else if (!strcmp(fifot, "moveright")) {
        Arg arg;
        moveright(arg);
    } else if (!strcmp(fifot, "movestart")) {
        Arg arg;
        movestart(arg);
    } else if (!strcmp(fifot, "moveend")) {
        Arg arg;
        moveend(arg);
    } else if (!strcmp(fifot, "movenext")) {
        Arg arg;
        movenext(arg);
    } else if (!strcmp(fifot, "moveprev")) {
        Arg arg;
        moveprev(arg);
    } else if (!strcmp(fifot, "viewhist")) {
        Arg arg;
        viewhist(arg);
    } else if (!strcmp(fifot, "backspace")) {
        Arg arg;
        backspace(arg);
    } else if (!strcmp(fifot, "deleteword")) {
        Arg arg;
        deleteword(arg);
    } else if (!strcmp(fifot, "moveword+")) {
        Arg arg;
        arg.i = +1;
        moveword(arg);
    } else if (!strcmp(fifot, "moveword-")) {
        Arg arg;
        arg.i = -1;
        moveword(arg);
    } else if (!strcmp(fifot, "movecursor+")) {
        Arg arg;
        arg.i = +1;
        movecursor(arg);
    } else if (!strcmp(fifot, "movecursor-")) {
        Arg arg;
        arg.i = -1;
        movecursor(arg);
    } else if (!strcmp(fifot, "clear")) {
        Arg arg;
        clear(arg);
    } else if (!strcmp(fifot, "exit_0")) {
        exit(0);
    } else if (!strcmp(fifot, "exit_1")) {
        exit(1);
    } else {
        fprintf(stderr, "majorna: Unknown command: %s\n", fifot);
    }

    close(fd);

    std::filesystem::remove(fifofile);
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
