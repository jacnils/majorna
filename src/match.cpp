/* See LICENSE file for copyright and license details. */

#include <regex/regex.h>

#include <match.hpp>
#include <majorna.hpp>
#include <cmath>

int matchregex(const char *t, const char *itt) {
    re_t reg = re_compile(t);
    int len;

    int ret = re_matchp(reg, itt, &len);

    if (ret != -1) {
        return 1;
    } else {
        return 0;
    }
}

void fuzzymatch() {
    struct item *it;
    struct item **fuzzymatches = nullptr;
    struct item *lhpprefix, *hpprefixend;
    lhpprefix = hpprefixend = nullptr;
    char c;
    int number_of_matches = 0, i, pidx, sidx, eidx;
    int text_len = strings.input_text.length(), itext_len;

    matches = matchend = nullptr;

    // walk through all items
    for (it = items; it && it->raw_text; it++) {
        if (text_len) {
            itext_len = strlen(it->raw_text);
            pidx = 0; // pointer
            sidx = eidx = -1; // start of match, end of match

            // walk through item text
            for (i = 0; i < itext_len && (c = it->raw_text[i]); i++) {
                if (!fstrncmp(&strings.input_text[pidx], &c, 1)) {
                    if(sidx == -1)
                        sidx = i;
                    pidx++;
                    if (pidx == text_len) {
                        eidx = i;
                        break;
                    }
                } else if (matchregex(strings.input_text.c_str(), it->raw_text) && regex) {
                    eidx = i;
                    break;
                }
            }

            // build list of matches
            if (eidx != -1) {
                it->distance = log(sidx + 2) + (double)(eidx - sidx - text_len);
                appenditem(it, &matches, &matchend);
                number_of_matches++;
            }
        } else {
            appenditem(it, &matches, &matchend);
        }
    }

    if (number_of_matches) {
        // initialize array with matches
        if (!(fuzzymatches = static_cast<item**>(realloc(fuzzymatches, number_of_matches * sizeof(struct item*)))))
            die("majorna: cannot realloc %u bytes:", number_of_matches * sizeof(struct item*));
        for (i = 0, it = matches; it && i < number_of_matches; i++, it = it->right) {
            fuzzymatches[i] = it;
        }
        // sort matches according to distance
        if (sortmatches) qsort(fuzzymatches, number_of_matches, sizeof(struct item*), compare_distance);

        // rebuild list of matches
        matches = matchend = nullptr;
        for (i = 0, it = fuzzymatches[i];  i < number_of_matches && it && \
                it->raw_text; i++, it = fuzzymatches[i]) {

            if (sortmatches && it->high_priority)
                appenditem(it, &lhpprefix, &hpprefixend);
            else
                appenditem(it, &matches, &matchend);
        }
        free(fuzzymatches);
    }

    if (lhpprefix) {
        hpprefixend->right = matches;
        matches = lhpprefix;
    }

    currentitem = selecteditem = matches;

    for (i = 0; i < preselected; i++) {
        if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
            currentitem = nextitem;
        }
    }

    calcoffsets();
}

void match() {
    get_width();

    if (fuzzy) {
        fuzzymatch();
        return;
    }

    static char **tokv = nullptr;
    static int tokn = 0;

    char buf[sizeof strings.input_text], *s;
    int i, tokc = 0;
    size_t len, textsize;
    struct item *item, *lhpprefix, *lprefix, *lsubstr, *hpprefixend, *prefixend, *substrend;


    sp_strncpy(buf, strings.input_text.c_str(), strings.input_text.size());
    // separate input text into tokens to be matched individually
    for (s = strtok(buf, " "); s; tokv[tokc - 1] = s, s = strtok(nullptr, " "))
        if (++tokc > tokn && !(tokv = static_cast<char**>(realloc(tokv, ++tokn * sizeof *tokv))))
            die("majorna: cannot realloc %u bytes:", tokn * sizeof *tokv);

    len = tokc ? strlen(tokv[0]) : 0;

    matches = lhpprefix = lprefix = lsubstr = matchend = hpprefixend = prefixend = substrend = nullptr;
    textsize = strings.input_text.length() + 1;
    for (item = items; item && item->raw_text; item++) {
        for (i = 0; i < tokc; i++)
            if (!fstrstr(item->raw_text, tokv[i]))
                break;
        if (i != tokc) // not all tokens match
            if (!(matchregex(strings.input_text.c_str(), item->raw_text) && regex)) {
                continue;
            }

        if (!sortmatches)
            appenditem(item, &matches, &matchend);
        else {
            // exact matches go first, then prefixes with high priority, then prefixes, then substrings
            if (!tokc || !fstrncmp(strings.input_text.c_str(), item->raw_text, textsize))
                appenditem(item, &matches, &matchend);
            else if (item->high_priority && !fstrncmp(tokv[0], item->raw_text, len))
                appenditem(item, &lhpprefix, &hpprefixend);
            else if (!fstrncmp(tokv[0], item->raw_text, len))
                appenditem(item, &lprefix, &prefixend);
            else
                appenditem(item, &lsubstr, &substrend);
        }
    }

    if (lhpprefix) {
        if (matches) {
            matchend->right = lhpprefix;
            lhpprefix->left = matchend;
        } else
            matches = lhpprefix;
        matchend = hpprefixend;
    }
    if (lprefix) {
        if (matches) {
            matchend->right = lprefix;
            lprefix->left = matchend;
        } else
            matches = lprefix;
        matchend = prefixend;
    }
    if (lsubstr) {
        if (matches) {
            matchend->right = lsubstr;
            lsubstr->left = matchend;
        } else
            matches = lsubstr;
        matchend = substrend;
    }

    currentitem = selecteditem = matches;

    for (i = 0; i < preselected; i++) {
        if (selecteditem && selecteditem->right && (selecteditem = selecteditem->right) == nextitem) {
            currentitem = nextitem;
        }
    }

    calcoffsets();
}

int compare_distance(const void *a, const void *b) {
    struct item *da = *(struct item **) a;
    struct item *db = *(struct item **) b;

    if (!db)
        return 1;
    if (!da)
        return -1;

    return da->distance == db->distance ? 0 : da->distance < db->distance ? -1 : 1;
}
