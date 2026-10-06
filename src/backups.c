//
// Created by Leo Pilcher on 8/31/26.
//
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include "crossplatmkdir.h"
#include "crossplatremove.h"
#include "savestuff.h"

#include "backups.h"

static bool IgnoresContains(const char *text) {
    for (size_t i = 0; i < IGNORES_LEN; i++) {
        if (IGNORES[i] != NULL && strcmp(IGNORES[i], text) == 0) {
            return true;
        }
    }
    return false;
}

static char *GetBackupDirName() {
    time_t rawTime; // Time type, raw time (not time zone specific)
    time(&rawTime); // Set raw time using time function
    const struct tm *timeInfo = localtime(&rawTime);

    char *dirName = malloc(SAFE_DIR_MAX);
    const size_t bytesWritten = strftime(dirName, SAFE_DIR_MAX - 1, BACKUP_NAME, timeInfo);
    // Only write as much as the buffer can hold (49 bytes)
    if (bytesWritten == 0) {
        fprintf(stderr, "Backup directory name too long\n");
        free(dirName);
        return NULL;
    }
    return dirName;
}

static char *MakeBackupDir() {
    DIR *destDir = opendir(TARGET_DIR);
    // Make destination directory if doesn't already exist
    if (destDir == NULL && errno == ENOENT) {
        make_dir(TARGET_DIR);
        destDir = opendir(TARGET_DIR);
        if (destDir == NULL) {
            fprintf(stderr, "Failed to create destination directory");
            exit(1);
        }
    }
    closedir(destDir);

    char *backupDirName = GetBackupDirName();

    size_t pathLen = strlen(TARGET_DIR) + 1 + strlen(backupDirName) + 1; // +1 for / and +1 for null-terminator

    char *path = (char *) malloc(pathLen);
    if (path == NULL) {
        fprintf(stderr, "Failed to allocate memory for backup path\n");
        free(backupDirName);
        return NULL;
    }
    snprintf(path, pathLen, "%s/%s", TARGET_DIR, backupDirName);
    free(backupDirName);

    errno = 0;
    DIR *backupDir = opendir(path);
    if (backupDir == NULL && errno == ENOENT) {
        make_dir(path);
        return path;
    }
    if (backupDir != NULL) {
        fprintf(stderr, "A backup named %s may already exist\n", path);
        closedir(backupDir);
    } else {
        fprintf(stderr, "Failed to open or check backup directory %s\n", path);
    }
    free(path);
    exit(0);
}

static char *MakeSubDir(char *parent, char *name) {
    // Check if parent dir exists
    DIR *parentDir;
    if ((parentDir = opendir(parent)) == NULL) {
        fprintf(stderr, "Failed to open parent dir\n");
        return NULL;
    }
    closedir(parentDir);

    size_t subDirPathLen = strlen(parent) + 1 + strlen(name) + 1; // Add 1 for '/' and one for null terminator
    char *subDirPath = malloc(subDirPathLen);
    snprintf(subDirPath, subDirPathLen, "%s/%s", parent, name);
    make_dir(subDirPath);
    return subDirPath;
}

static int CopyFile(const char *const src, const char *const dest, const char *const filename) {
    // Returns 0 for success, 1 for failure
    size_t destFilePathLen = strlen(dest) + 1 + strlen(filename) + 1;
    // Add one for / and one for null terminator
    char *destFilePath = malloc(destFilePathLen);
    snprintf(destFilePath, destFilePathLen, "%s/%s", dest, filename);
    FILE *destFile = fopen(destFilePath, "wb");
    if (destFile == NULL) {
        fprintf(stderr, "Failed to create destination file\n");
        free(destFilePath);
        return 1;
    }
    size_t srcFilePathLen = strlen(src) + 1 + strlen(filename) + 1;
    char *srcFilePath = malloc(srcFilePathLen);
    snprintf(srcFilePath, srcFilePathLen, "%s/%s", src, filename);
    FILE *srcFile = fopen(srcFilePath, "rb");
    if (srcFile == NULL) {
        fprintf(stderr, "Failed to read src file");
        free(destFilePath);
        fclose(destFile);
        free(srcFilePath);
        return 1;
    }

    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0) {
        fwrite(buffer, 1, bytesRead, destFile);
    }

    fclose(destFile);
    fclose(srcFile);
    free(srcFilePath);
    free(destFilePath);
    return 0;
}

static void CopyDirContentsRecursive(char *src, char *dest) {
    struct dirent *srcEntry;
    DIR *srcDir = opendir(src);
    if (srcDir == NULL) {
        fprintf(stderr, "Failed to open source directory\n");
        exit(1);
    }

    while ((srcEntry = readdir(srcDir)) != NULL) {
        if (strcmp(srcEntry->d_name, ".") == 0 || strcmp(srcEntry->d_name, "..") == 0) continue;
        // Skip over . and .. directories to avoid infinite recursion
        if (IgnoresContains(srcEntry->d_name)) continue;
        // Add one for '/' and one for null terminator
        const size_t pathToEntryLength = strlen(src) + strlen(srcEntry->d_name) + 2;
        char *pathToEntry = malloc(pathToEntryLength);
        snprintf(pathToEntry, pathToEntryLength, "%s/%s", src, srcEntry->d_name);
        if (srcEntry->d_type == DT_DIR) {
            char *subDir = MakeSubDir(dest, srcEntry->d_name);
            if (subDir == NULL) {
                fprintf(stderr, "Failed to create subdirectory in destination directory\n");
                closedir(srcDir);
                return;
            }
            CopyDirContentsRecursive(pathToEntry, subDir);
            free(subDir);
        }
        else if (srcEntry->d_type == DT_REG) {
            // Continue if success. Otherwise, close dir and exit with error
            if (CopyFile(src, dest, srcEntry->d_name) == 0) continue;
            closedir(srcDir);
            free(pathToEntry);
            fprintf(stderr, "Failed to copy file %s", srcEntry->d_name);
            exit(1);
        }
        else if (srcEntry->d_type == DT_LNK) {
            char symlinkPointsTo[SAFE_PATH_MAX];
            realpath(pathToEntry, symlinkPointsTo);
            printf("Symlink %s \tPointing To: %s\n", pathToEntry, symlinkPointsTo);
        }
        free(pathToEntry);
    }
    closedir(srcDir);
}

void MakeBackup() {
    char *backupDirPath = MakeBackupDir();

    if (backupDirPath == NULL) {
        fprintf(stderr, "Failed to create backup directory");
        exit(1);
    }
    CopyDirContentsRecursive(".", backupDirPath);

    free(backupDirPath);
}

static void ClearDirContentsRecursive(char *path) {
    struct dirent *dirEntry;
    DIR *currentDir = opendir(path);

    if (currentDir == NULL) {
        fprintf(stderr, "Failed to clear directory\n");
        printf("%i, %s\n", errno, path);
        exit(1);
    }

    while ((dirEntry = readdir(currentDir)) != NULL) {
        // Skip over . and .. directories to avoid infinite recursion
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) continue;
        // Calculate size of path to current entry being removed. +1 for / +1 for null terminator
        const size_t pathToEntrySize = strlen(path) + 1 + strlen(dirEntry->d_name) + 1;
        char *pathToEntry = malloc(pathToEntrySize);
        snprintf(pathToEntry, pathToEntrySize, "%s/%s", path, dirEntry->d_name);
        const int err = cross_plat_remove(pathToEntry);
        // Continue to next entry if deletion was successful
        if (err == 0) { free(pathToEntry); continue; }
        switch (errno) {
            case ENOTEMPTY:  // Directory not empty
                ClearDirContentsRecursive(pathToEntry);
                break;
            case EACCES: // Permission denied
            case EPERM:  // Operation not permitted
            case EROFS:  // Read only file system
                fprintf(stderr, "Not allowed to delete item %s\n", pathToEntry);
                free(pathToEntry);
                closedir(currentDir);
                exit(1);
            default:
                fprintf(stderr, "Error %i while deleting file or directory %s\n", errno, pathToEntry);
                free(pathToEntry);
                closedir(currentDir);
                exit(1);
        }
        free(pathToEntry);
    }
    closedir(currentDir);
}

void RestoreBackup() {
    ClearDirContentsRecursive(".");
    CopyDirContentsRecursive(BACKUP_TO_RESTORE, ".");
}
