/* See LICENSE file for copyright and license details. */

#include <cstring>
#include <sort.hpp>
#include <majorna.hpp>

std::vector<std::string> tokenize(const std::string& source, const char* delim) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = source.find_first_of(delim);

    while (end != std::string::npos) {
        tokens.push_back(source.substr(start, end - start));
        start = end + 1;
        end = source.find_first_of(delim, start);
    }
    tokens.push_back(source.substr(start));

    return tokens;
}

int arrayhas(char **list, int length, char *item) {
    int i;

    for (i = 0; i < length; i++) {
        size_t len1 = strlen(list[i]);
        size_t len2 = strlen(item);
        if (!fstrncmp(list[i], item, len1 > len2 ? len2 : len1))
            return 1;
    }
    return 0;
}
