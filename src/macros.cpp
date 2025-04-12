/* See LICENSE file for copyright and license details. */

#include <iostream>
#include <errno.h>
#include <time.h>
#include <macros.hpp>
#include <cstring>
#include <cstdarg>

size_t sp_strncpy(char* dst, const char* src, size_t size) {
    int offset;

    offset = 0;

    if (size > 0) {
        while (*(src+offset) != '\0' ) {
            if (offset == size) {
                offset--;
                break;
            }

            *(dst+offset) = *(src+offset);
            offset++;
        }
    }

    *(dst+offset) = '\0';

    while (*(src+offset) != '\0')
        offset++;

    return offset;
}

void die(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
        fputc(' ', stderr);
        perror(NULL);
    } else {
        fputc('\n', stderr);
    }

    exit(1);
}

void *ecalloc(size_t nmemb, size_t size) {
    void *p;

    if (!(p = calloc(nmemb, size)))
        die("calloc:");
    return p;
}

int msleep(long sec) {
    struct timespec ts;
    int ret;

    if (sec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = sec / 1000;
    ts.tv_nsec = (sec % 1000) * 1000000;

    do {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}
