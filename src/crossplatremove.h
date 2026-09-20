//
// Created by Leo Pilcher on 9/16/26.
//

#ifndef SAVESTUFF_CROSSPLATREMOVE_H
#define SAVESTUFF_CROSSPLATREMOVE_H

#include <stdio.h>
#include <errno.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <direct.h>   // For _rmdir
    #include <windows.h>  // For GetFileAttributesA
#else
    #include <unistd.h>   // For POSIX systems
#endif

static int cross_plat_remove(const char *path) {
#if defined(_WIN32) || defined(_WIN64)
    // Try deleting as a file first
    if (remove(path) == 0) {
        return 0;
    }

    // Check if windows remove failed possibly due to passing a directory
    if (errno == EACCES) {
        DWORD attrs = GetFileAttributesA(path);
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            // If file is a directory, delete it using _rmdir and return
            if (_rmdir(path) == 0) {
                return 0;
            }
            // If _rmdir failed, it appropriately sets errno
            return -1;
        }
        // If the problem is actually no access then set errno and return
        errno = EACCES;
    }
    return -1;
#else
    // Linux/macOS standard remove() inherently deletes files and empty directories
    return remove(path);
#endif
}


#endif //SAVESTUFF_CROSSPLATREMOVE_H
