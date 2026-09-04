//
// Created by Leo Pilcher on 8/31/26.
//
#pragma once

#include <limits.h>

#ifndef SAVESTUFF_CROSSPLATLIMITS_H
#define SAVESTUFF_CROSSPLATLIMITS_H

#ifdef PATH_MAX
    #define SAFE_PATH_MAX (PATH_MAX + 1)
#elif defined(_MAX_PATH)
    #define SAFE_PATH_MAX (_MAX_PATH + 1)
#else
    #define SAFE_PATH_MAX 4097
#endif

#ifdef NAME_MAX
    #define SAFE_DIR_MAX (NAME_MAX + 1)
#elif defined(_MAX_FNAME)
    #define SAFE_DIR_MAX (_MAX_FNAME + 1)
#else
    #define SAFE_DIR_MAX 256
#endif

#endif //SAVESTUFF_CROSSPLATLIMITS_H