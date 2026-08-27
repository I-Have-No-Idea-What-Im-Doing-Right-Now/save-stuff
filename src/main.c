//
// Created by Leo Pilcher on 8/22/26.
//

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

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

static void PrintHelp(int messageNum) {
    #include <generated/generalhelp.h>
    switch (messageNum) {
        case 0:
            fwrite(general_txt, sizeof(char), general_txt_len, stdout);
            break;
        case 1:
            break;
        default:
            fwrite(general_txt, sizeof(char), general_txt_len, stdout);
            break;
    }
    putchar('\n');
}

static int GetHelpMessageNum(const char* string, const size_t len) {
    if (string[len] != 0) {
        fprintf(stderr, "GetHelpMessageNum: must input null-terminated string");
        exit(1);
    }

    if (strcmp(string, "version") == 0) {
        return 1;
    }
    return 0;
}

static char *GetBackupDirName() {
    time_t rawTime; // Time type, raw time (not time zone specific)
    time(&rawTime); // Set raw time using time function
    struct tm *timeInfo = localtime(&rawTime);

    char *dirName = malloc(50);
    strftime(dirName, 49, BACKUP_NAME, timeInfo); // Only write as much as the buffer can hold (49 bytes)
    return dirName;
}

static char *MakeBackupDir() {
    DIR *destDir = opendir(TARGET_DIR);
    // Make destination directory if doesn't already exist
    if (destDir == NULL) {
        make_dir(TARGET_DIR);
        destDir = opendir(TARGET_DIR);
        if (destDir == NULL) {
            fprintf(stderr, "Failed to open destination directory");
            exit(1);
        }
    }
    closedir(destDir);

    char *backupDirName = GetBackupDirName();

    size_t pathLen = strlen(TARGET_DIR) + 1 + strlen(backupDirName) + 1; // +1 for / and +1 for null-terminator

    char *path = (char *)malloc(pathLen);
    if (path == NULL) {
        fprintf(stderr, "Failed to allocate memory for backup path");
        exit(1);
    }
    snprintf(path, pathLen, "%s/%s", TARGET_DIR, backupDirName);
    free(backupDirName);

    make_dir(path);
    return path;
}

static void BackUpDir() {
    DIR *srcDir = opendir(".");
    char *backupDirPath = MakeBackupDir();
    struct dirent *entry;
    if (srcDir == NULL) {
        fprintf(stderr, "Failed to open directory");
        exit(1);
    }
    free(backupDirPath);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        BackUpDir();
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "help") == 0 ||
            strcmp(argv[i], "--help") == 0 ||
            strcmp(argv[i], "-h") == 0) {
            const char* prevArg = argv[i-1 >=0 ? i-1 : 0]; // Get previous argument and make sure it is in bounds
            PrintHelp(GetHelpMessageNum(prevArg, strlen(prevArg)));
            return 0;
        }
        if (strcmp(argv[i], "version") == 0 ||
            strcmp(argv[i], "-v") == 0 ||
            strcmp(argv[i], "--version") == 0) {
            PrintVersion();
            return 0;
        }
        if (strcmp(argv[i], "-o") == 0 ||
                 strcmp(argv[i], "--out") == 0) {
            const char* nextArg = i + 1 <= argc - 1 ? argv[i+1] : NULL;
            if (nextArg == NULL) {
                fprintf(stderr, "No value passed for -o / --out option\n");
                exit(1);
            }
            memset(BACKUP_NAME, 0, sizeof(BACKUP_NAME)); // Clear the string
            strncpy(BACKUP_NAME, nextArg, sizeof(BACKUP_NAME) - 1); // Write the name into the string
        }
    }
    BackUpDir();
    return 0;
}
