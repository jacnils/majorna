#pragma once

#include <cstddef>

inline char** history;
inline size_t histsz, histpos;

void load_history();
void navigatehistfile(int dir);
void savehistory(char* hist);
