/* See LICENSE file for copyright and license details. */

#include <stream.hpp>
#include <sort.hpp>
#include <majorna.hpp>
#include <img.hpp>

void readstdin() {
    char buf[sizeof tx.text], *p;
    size_t i, itemsiz = 0;
    unsigned int tmpmax = 0;

    if (passwd) {
        sp.inputw = lines = 0;
        return;
    }

    if (listfile) {
        readfile();
        return;
    }

    int o = 0;

    // read each line from stdin and add it to the item list
    for (i = 0; fgets(buf, sizeof buf, stdin); i++) {
        if (i + 1 >= itemsiz) {
            itemsiz += 256;
            if (!(items = static_cast<item*>(realloc(items, itemsiz * sizeof(*items)))))
                die("majorna: cannot realloc %zu bytes:", itemsiz * sizeof(*items));
        }
        if ((p = strchr(buf, '\n')))
            *p = '\0';
        if (!(items[i].text = strdup(buf)))
            die("majorna: cannot strdup %u bytes:", strlen(buf) + 1);
        items[i].hp = arrayhas(hpitems, hplength, items[i].text);
        const auto tmpmax = draw.get_font_manager().estimate_length(buf, true).first; // width
        if (tmpmax > sp.inputw) {
            sp.inputw = tmpmax;
        }

        items[i].index = i;

        if (parsemarkup(i)) {
            o = 1;
        }

#if !IMAGE
        if (o) {
            ;
        }
#endif
    }

#if IMAGE
    if (!o) img.longestedge = img.imagegaps = 0;
#endif

    // clean
    if (items) {
        items[i].text = nullptr;
#if IMAGE
        items[i].image = nullptr;
#endif
    }

    lines = MAX(MIN(lines, i), minlines);
}

void readfile() {
    if (passwd){
        sp.inputw = lines = 0;
        return;
    }

    size_t len;
    static size_t c = 0;
    char *l;

    FILE *lf = fopen(listfile, "r");

    if (!lf) return;

    items = nullptr;
    listsize = 0;

    for (;;) {
        l = nullptr;
        len = 0;

        if (-1 == getline(&l, &len, lf)) {
            if (ferror(lf)) die("majorna: failed to read file\n");
            free(l);
            break;
        }

        if (c == listsize) {
            c += 64 * sizeof(char*);
            list = static_cast<char**>(realloc(list, c));
            if (!list) die("majorna: failed to realloc memory");
        }

        strtok(l, "\n");
        list[listsize] = l;
        listsize++;
    }

    if (fclose(lf)) {
        die("majorna: failed to close file %s\n", listfile);
    }

    if (!list_items) {
        list_items = items;
        items = static_cast<item*>(calloc(listsize + 1, sizeof(struct item)));
        if (!items) die("majorna: cannot alloc memory\n");

        int i = 0;
        int o = 0;

        for (i = 0; i < listsize; i++) {
            items[i].text = list[i];

            if (parsemarkup(i)) {
                o = 1;
            }

#if !IMAGE
            if (o) {
                ;
            }
#endif
        }

        lines = MAX(columns == 1 ? i : MIN(i, lines), minlines);

#if IMAGE
        if (!o) img.longestedge = img.imagegaps = 0;
#endif

        if (i == sp.listcount) {
            sp.listchanged = 0;
            sp.listcount = i;
        } else {
            sp.listcount = i;
            sp.listchanged = 1;

            // prevents state->buffer from being nullptr
            if (!protocol) {
                resizeclient();
            }
        }
    } else {
        free(items);
        items = list_items;
        list_items = nullptr;
    }
}

int parsemarkup(int index) {
#if IMAGE
    int w, h;
    char *limg = nullptr;
#endif

    // parse image markup
#if IMAGE
    if (!strncmp("IMG:", items[index].text, strlen("IMG:")) || !strncmp("img://", items[index].text, strlen("img://"))) {
        if(!(items[index].image = static_cast<char*>(malloc(strlen(items[index].text)+1))))
            fprintf(stderr, "majorna: cannot malloc %lu bytes\n", strlen(items[index].text));
        if (sscanf(items[index].text, "IMG:%[^\t]", items[index].image)) {
            items[index].text += strlen("IMG:")+strlen(items[index].image)+1;
        } else if (sscanf(items[index].text, "img://%[^\t]", items[index].image)) {
            items[index].text += strlen("img://")+strlen(items[index].image)+1;
        } else {
            free(items[index].image);
            items[index].image = nullptr;
        }
    } else {
        items[index].image = nullptr;
    }

    // load image cache (or generate)
    if (generatecache && img.longestedge <= maxcache && items[index].image && strcmp(items[index].image, limg ? limg : "")) {
        loadimagecache(items[index].image, &w, &h);
    }

    if(items[index].image) {
        limg = items[index].image;
    }

    if (limg) {
        return 1;
    } else {
        return 0;
    }
#else // remove the data, just for convenience
    char *data;

    if (!strncmp("IMG:", items[index].text, strlen("IMG:")) || !strncmp("img://", items[index].text, strlen("img://"))) {
        if(!(data = static_cast<char*>(malloc(strlen(items[index].text)+1))))
            fprintf(stderr, "majorna: cannot malloc %lu bytes\n", strlen(items[index].text));
        if(sscanf(items[index].text, "IMG:%[^\t]", data)) {
            items[index].text += strlen("IMG:")+strlen(data)+1;
        } else if(sscanf(items[index].text, "img://%[^\t]", data)) {
            items[index].text += strlen("img://")+strlen(data)+1;
        }
    }
#endif
}
