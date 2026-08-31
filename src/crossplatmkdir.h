//
// Created by Leo Pilcher on 8/31/26.
//

#ifndef SAVESTUFF_CROSSPLATMKDIR_H
#define SAVESTUFF_CROSSPLATMKDIR_H

// --- CROSS-PLATFORM SETUP FOR MAKE DIR ---
#if defined(_WIN32) || defined(_WIN64)
    #include <direct.h>     // Windows header for _mkdir
    #define make_dir(path) _mkdir(path)
#else
    #include <sys/stat.h>   // POSIX header for mkdir
    #include <sys/types.h>
    // Linux/macOS requires permissions; 0777 means full access
    #define make_dir(path) mkdir(path, 0777)
#endif
// ----------------------------

#endif //SAVESTUFF_CROSSPLATMKDIR_H
