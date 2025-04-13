/* See LICENSE file for copyright and license details. */

#if RTL
#include <fribidi.h>
#endif
#include <cstring>
#include <string>

#include <rtl.hpp>

#if RTL
void apply_fribidi(std::string& str) {
    FriBidiStrIndex len = str.length();
    FriBidiChar logical[BUFSIZ];
    FriBidiChar visual[BUFSIZ];
    FriBidiParType base = FRIBIDI_PAR_ON;
    FriBidiCharSet charset;

    char fribidi_text[BUFSIZ] = "";

    if (len > 0) {
        charset = fribidi_parse_charset("UTF-8");
        len = fribidi_charset_to_unicode(charset, str.c_str(), len, logical);

        static_cast<void>(fribidi_log2vis(logical, len, &base, visual, nullptr, nullptr, nullptr));
        static_cast<void>(fribidi_unicode_to_charset(charset, visual, len, fribidi_text));
    }
}

void apply_fribidi(char* str) {
    FriBidiStrIndex len = strlen(str);
    FriBidiChar logical[BUFSIZ];
    FriBidiChar visual[BUFSIZ];
    FriBidiParType base = FRIBIDI_PAR_ON;
    FriBidiCharSet charset;

    char fribidi_text[BUFSIZ] = "";

    if (len > 0) {
        charset = fribidi_parse_charset("UTF-8");
        len = fribidi_charset_to_unicode(charset, str, len, logical);

        static_cast<void>(fribidi_log2vis(logical, len, &base, visual, nullptr, nullptr, nullptr));
        static_cast<void>(fribidi_unicode_to_charset(charset, visual, len, fribidi_text));
    }
}
#else
void apply_fribidi(char *str) {
    return;
}
void apply_fribidi(std::string& str) {
    return;
}
#endif
