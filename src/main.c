//
// Created by Leo Pilcher on 8/22/26.
//

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>
#include <time.h>
#include <errno.h>
#include "commands.h"

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

static char const SAVESTUFF_VERSION[64] = "1.0.0";

static char TARGET_DIR[SAFE_PATH_MAX] = "../Backups";
static char BACKUP_NAME[SAFE_DIR_MAX] = "Backup_%Y-%m-%d_%H-%M-%S";

static void PrintVersion() {
    printf("%.64s\n", SAVESTUFF_VERSION);
}

static void PrintHelp(enum Commands command) {
    #include <generated/generalhelp.h>
    #include <generated/versionhelp.h>
    #include <generated/backuphelp.h>
    switch (command) {
        case COMMANDS_HELP_GENERAL:
            fwrite(general_txt, sizeof(char), general_txt_len, stdout);
            break;
        case COMMANDS_HELP_VERSION:
            fwrite(version_txt, sizeof(char), version_txt_len, stdout);
            break;
        case COMMANDS_HELP_BACKUP:
            fwrite(backup_txt, sizeof(char), backup_txt_len, stdout);
            break;
        case COMMANDS_HELP_RESTORE:
            break;
        default:
            return;
    }
    putchar('\n');
}

static enum Commands GetHelpMessageCommand(const char* string, const size_t len) {
    if (string[len] != 0) {
        fprintf(stderr, "GetHelpMessageNum: must input null-terminated string");
        exit(1);
    }

    if (strcmp(string, "version") == 0) {
        return COMMANDS_HELP_VERSION;
    }
    if (strcmp(string, "backup") == 0) {
        return COMMANDS_HELP_BACKUP;
    }
    if (strcmp(string, "restore") == 0) {
        return COMMANDS_HELP_RESTORE;
    }
    return COMMANDS_HELP_GENERAL;
}

static char *GetBackupDirName() {
    time_t rawTime; // Time type, raw time (not time zone specific)
    time(&rawTime); // Set raw time using time function
    struct tm *timeInfo = localtime(&rawTime);

    char *dirName = malloc(SAFE_DIR_MAX);
    const size_t bytesWritten = strftime(dirName, SAFE_DIR_MAX - 1, BACKUP_NAME, timeInfo); // Only write as much as the buffer can hold (49 bytes)
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
    if (errno == ENOENT) {
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

    char *path = (char *)malloc(pathLen);
    if (path == NULL) {
        fprintf(stderr, "Failed to allocate memory for backup path\n");
        free(backupDirName);
        return NULL;
    }
    snprintf(path, pathLen, "%s/%s", TARGET_DIR, backupDirName);
    free(backupDirName);

    DIR *backupDir = opendir(path);
    if (errno == ENOENT) {
        make_dir(path);
        return path;
    }
    fprintf(stderr, "A backup named %s may already exist", path);
    free(path);
    closedir(backupDir);
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

static void CopyDirContentsRecursive(char *src, char *dest) {
    struct dirent *srcEntry;
    DIR *srcDir = opendir(src);
    if (srcDir == NULL) {
        fprintf(stderr, "Failed to open source directory\n");
        exit(1);
    }

    while ((srcEntry = readdir(srcDir)) != NULL) {
        if (strcmp(srcEntry->d_name, ".") == 0 || strcmp(srcEntry->d_name, "..") == 0) continue; // Skip over
        if (srcEntry->d_type == DT_DIR) {
            char *subDir = MakeSubDir(dest, srcEntry->d_name);
            const size_t srcSubdirPathLen = strlen(src) + 1 + strlen(srcEntry->d_name) + 1; // Add one for / and one for null terminator
            char *srcSubdirPath = malloc(srcSubdirPathLen);
            snprintf(srcSubdirPath, srcSubdirPathLen, "%s/%s", src, srcEntry->d_name);
            if (subDir == NULL) {
                fprintf(stderr, "Failed to create subdirectory in destination directory\n");
                return;
            }
            CopyDirContentsRecursive(srcSubdirPath, subDir);
            free(subDir);
        }
        if (srcEntry->d_type == DT_REG) {
            size_t destFilePathLen = strlen(dest) + 1 + strlen(srcEntry->d_name) + 1; // Add one for / and one for null terminator
            char *destFilePath = malloc(destFilePathLen);
            snprintf(destFilePath, destFilePathLen, "%s/%s", dest, srcEntry->d_name);
            FILE *destFile = fopen(destFilePath, "wb");
            if (destFile == NULL) {
                fprintf(stderr, "Failed to create file\n");
                exit(1);
            }
            size_t srcFilePathLen = strlen(src) + 1 + strlen(srcEntry->d_name) + 1;
            char *srcFilePath = malloc(srcFilePathLen);
            snprintf(srcFilePath, srcFilePathLen, "%s/%s", src, srcEntry->d_name);
            FILE *srcFile = fopen(srcFilePath, "rb");
            if (srcFile == NULL) {
                fprintf(stderr, "Failed to read src file");
                exit(1);
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
        }
    }
}

static void MakeBackup() {
    char *backupDirPath = MakeBackupDir();

    if (backupDirPath == NULL) {
        fprintf(stderr, "Failed to create backup directory");
        exit(1);
    }
    CopyDirContentsRecursive(TARGET_DIR, backupDirPath);

    free(backupDirPath);
}

static char *GetPrevArg(char *argv[], const int i) {
    return argv[i-1 >=0 ? i-1 : 0]; // Get previous argument and make sure it is in bounds
}

static char *GetNextArg(const int argc, char *argv[], const int i) {
    return i + 1 <= argc - 1 ? argv[i+1] : NULL;
}

static void SetBackupName(const char *name) {
    memset(BACKUP_NAME, 0, sizeof(BACKUP_NAME)); // Clear the string
    strncpy(BACKUP_NAME, name, sizeof(BACKUP_NAME) - 1); // Write the name into the string
}

static void SetBackupLocation(const char *location) {
    memset(TARGET_DIR, 0, sizeof(TARGET_DIR));
    strncpy(TARGET_DIR, location, sizeof(TARGET_DIR));
}

static enum Commands ParseArgs(const int argc, char *argv[]) {
    enum Commands out = COMMANDS_NONE;
    if (argc == 1) {
        return out;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "help") == 0 ||
        strcmp(argv[i], "--help") == 0 ||
        strcmp(argv[i], "-h") == 0) {
            const char* prevArg = GetPrevArg(argv, i);
            return GetHelpMessageCommand(prevArg, strlen(prevArg));
        }

        if (strcmp(argv[i], "version") == 0 ||
        strcmp(argv[i], "-v") == 0 ||
        strcmp(argv[i], "--version") == 0) {
            out = COMMANDS_VERSION;
        }

        if (strcmp(argv[i], "backup") == 0) {
            out = COMMANDS_BACKUP;
        }

        if (strcmp(argv[i], "restore") == 0) {
            out = COMMANDS_RESTORE;
        }

        if (strcmp(argv[i], "-o") == 0 ||
        strcmp(argv[i], "--out") == 0) {
            const char* nextArg = GetNextArg(argc, argv, i);
            if (nextArg == NULL) {
                fprintf(stderr, "No value passed for -o / --out option\n");
                exit(1);
            }
            if ((i + 1) < argc) { // Skip next argument if possible so it is not interpreted as a command
                i++;
            }
            else {
                break;
            }
            SetBackupName(nextArg);
        }

        if (strcmp(argv[i], "-l") == 0 ||
        strcmp(argv[i], "--location") == 0) {
            const char* nextArg = GetNextArg(argc, argv, i);
            if (nextArg == NULL) {
                fprintf(stderr, "No value passed for -o / --out option\n");
                exit(1);
            }
            if ((i + 1) < argc) { // Skip next argument if possible so it is not interpreted as a command
                i++;
            }
            else {
                break;
            }
            SetBackupLocation(nextArg);
        }
    }
    return out;
}

int main(const int argc, char *argv[]) {
    enum Commands command = ParseArgs(argc, argv);
    PrintHelp(command);
    switch (command) {
        case COMMANDS_BACKUP:
            MakeBackup();
            printf("Successfully created backup\n");
            break;
        case COMMANDS_VERSION:
            PrintVersion();
            break;
        case COMMANDS_RESTORE:
            break;
        default:
            break;
    }
    return 0;
}