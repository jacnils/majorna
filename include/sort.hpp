#pragma once

char **tokenize(char *source, const char *delim, int *llen);
int arrayhas(char **list, int length, char *item);