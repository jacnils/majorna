#pragma once

#include <string>
#include <vector>

std::vector<std::string> tokenize(const std::string& source, const char* delim);
int arrayhas(char **list, int length, char *item);