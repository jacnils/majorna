/* See LICENSE file for copyright and license details. */

#include <history.hpp>
#include <macros.hpp>
#include <cstring>
#include <cstdio>
#include <options.hpp>
#include <majorna.hpp>
#include <match.hpp>
#include <filesystem>

void load_history() {
    FILE *fp = nullptr;
    static size_t cap = 0;
    size_t llen;
    char *line;

    // no history file, give up like i do with all things in life
    if (histfile.empty() || !std::filesystem::is_regular_file(histfile)) {
        return;
    }

    // open history file, return if it failed
    fp = fopen(histfile.c_str(), "r");
    if (!fp) {
        fprintf(stderr, "majorna: failed to open history file\n");
        return;
    }

    for (;;) {
        line = nullptr;
        llen = 0;

        if (-1 == getline(&line, &llen, fp)) {
            if (ferror(fp)) {
                die("majorna: failed to read history");
            }

            free(line);
            break;
        }

        if (cap == histsz) {
            cap += 64 * sizeof(char*);
            history = static_cast<char**>(realloc(history, cap));
            if (!history) {
                die("majorna: failed to realloc memory");
            }
        }
        strtok(line, "\n");
        history[histsz] = line;
        histsz++;
    }

    histpos = histsz;

    if (fclose(fp)) {
        die("majorna: failed to close file %s", histfile);
    }
}

void navigatehistfile(int dir) {
    static char def[BUFSIZ];
    char *p = nullptr;
    size_t len = 0;

    if (!history || histpos + 1 == 0)
        return;

    if (histsz == histpos) {
        strncpy(def, strings.input_text, sizeof(def));
    }

    switch (dir) {
        case 1:
            if (histpos < histsz - 1) {
                p = history[++histpos];
            } else if (histpos == histsz - 1) {
                p = def;
                histpos++;
            }
            break;
        case -1:
            if (histpos > 0) {
                p = history[--histpos];
            }
            break;
    }

    if (p == nullptr) {
        return;
    }

    len = MIN(strlen(p), BUFSIZ - 1);
    sp_strncpy(strings.input_text, p, sizeof(strings.input_text));
    strings.input_text[len] = '\0';
    ctx.cursor = len;
    match();
}

void savehistory(char *input) {
    unsigned int i;
    FILE *fp;

    if (histfile.empty() ||
            0 == maxhist ||
            0 == strlen(input)) {
        goto out;
    }

    fp = fopen(histfile.c_str(), "w");
    if (!fp) {
        die("majorna: failed to open %s", histfile);
    }
    for (i = histsz < maxhist ? 0 : histsz - maxhist; i < histsz; i++) {
        if (0 >= fprintf(fp, "%s\n", history[i])) {
            die("majorna: failed to write to %s", histfile);
        }
    }
    if (histsz == 0 || histdup || (histsz > 0 && strcmp(input, history[histsz-1]) != 0)) {
        if (0 >= fputs(input, fp)) {
            die("majorna: failed to write to %s", histfile);
        }
    }
    if (fclose(fp)) {
        die("majorna: failed to close file %s", histfile);
    }

out:
    for (i = 0; i < histsz; i++) {
        free(history[i]);
    }
    free(history);
}
