#pragma once

#include <cstddef>

#ifndef INTERSECT
#define INTERSECT(x,y,w,h,r)    (MAX(0, MIN((x)+(w),(r).x_org+(r).width)  - MAX((x),(r).x_org)) \
        && MAX(0, MIN((y)+(h),(r).y_org+(r).height) - MAX((y),(r).y_org)))
#endif
#ifndef TEXTW
#define TEXTW(X)                (draw.get_text_width((X), false) + ctx.lrpad)
#endif
#ifndef TEXTWM
#define TEXTWM(X)               (draw.get_text_width((X), true) + ctx.lrpad)
#endif
#ifndef NUMBERSMAXDIGITS
#define NUMBERSMAXDIGITS        100
#endif
#ifndef NUMBERSBUFSIZE
#define NUMBERSBUFSIZE          (NUMBERSMAXDIGITS * 2) + 1
#endif
#ifndef MAXITEMLENGTH
#define MAXITEMLENGTH           1024
#endif

size_t sp_strncpy(char* dst, const char* src, size_t size);
void die(const char *fmt, ...);
void *ecalloc(size_t nmemb, size_t size);
int msleep(long sec);
