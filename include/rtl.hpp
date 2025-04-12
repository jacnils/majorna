#pragma once

#if RTL
#include <fribidi.h>
#include <cstring>
#endif
#include <cstdio>

inline char fribidi_text[BUFSIZ] = "";

void apply_fribidi(char *str);
