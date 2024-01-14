#pragma once

#include "bool.h"

#define CREATE_PATH ".cache.backup"

typedef struct mapconfig_t {
    int total_limit;
    int dirs_limit;

    void (*fptr)(char *realpath);
} mapconfig_t;

typedef struct dirctx_t {
    char **array;
    size_t array_size;
    size_t new_array_size;
    size_t total_dirs_ran;
    BOOL ran;

    mapconfig_t *config;
} dirctx_t;

void MapScanAll(mapconfig_t *config);
void MapScanFromArr(mapconfig_t *config, char *Arr[], size_t Size);
