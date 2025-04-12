#pragma once

#include <sys/types.h>

void randname(char *buf);
int anonymous_shm_open();
int create_shm_file(off_t size);