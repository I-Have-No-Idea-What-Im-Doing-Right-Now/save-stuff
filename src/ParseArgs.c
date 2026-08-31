//
// Created by Leo Pilcher on 8/31/26.
//
#include <stdio.h>

#include "commands.h"
#include <limits.h>
#include <string.h>
#include "savestuff.h"
#include "ParseArgs.h"

static enum Commands GetHelpMessageCommand(const char* string, const size_t len) {
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

static void SetBackupName(const char *name) {
    memset(BACKUP_NAME, 0, sizeof(BACKUP_NAME)); // Clear the string
    strncpy(BACKUP_NAME, name, sizeof(BACKUP_NAME) - 1); // Write the name into the string
}

static void SetBackupLocation(const char *location) {
    memset(TARGET_DIR, 0, sizeof(TARGET_DIR));
    strncpy(TARGET_DIR, location, sizeof(TARGET_DIR));
}

static void SetRestoreBackup(const char *backup) {
    memset(BACKUP_TO_RESTORE, 0, sizeof(BACKUP_TO_RESTORE));
    strncpy(BACKUP_TO_RESTORE, backup, sizeof(BACKUP_TO_RESTORE));
}

static void AppendIgnores(const char *text) {
    IGNORES_LEN++;
    char **temp = realloc(IGNORES, IGNORES_LEN * sizeof(char *));
    if (temp == NULL) {
        fprintf(stderr, "Failed to allocate memory for new ignore entry");
        exit(0);
    }
    IGNORES = temp;

    IGNORES[IGNORES_LEN - 1] = (char *)malloc(NAME_MAX);
    if (IGNORES[IGNORES_LEN - 1] != NULL) {
        strncpy(IGNORES[IGNORES_LEN - 1], text, NAME_MAX);
    }
}

static char *GetPrevArg(char *argv[], const int i) {
    return argv[i-1 >=0 ? i-1 : 0]; // Get previous argument and make sure it is in bounds
}

static char *GetNextArg(const int argc, char *argv[], const int i) {
    return i + 1 <= argc - 1 ? argv[i+1] : NULL;
}

enum Commands ParseArgs(const int argc, char *argv[]) {
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
        if (strcmp(argv[i], "-i") == 0 ||
        strcmp(argv[i], "--ignore") == 0) {
            const char* nextArg = GetNextArg(argc, argv, i);
            if (nextArg == NULL) {
                fprintf(stderr, "No value passed for -i / --ignore option\n");
                exit(1);
            }
            if ((i + 1) < argc) { // Skip next argument if possible so it is not interpreted as a command
                i++;
            }
            else {
                break;
            }
            AppendIgnores(nextArg);
        }
        if (strcmp(argv[i], "-b") == 0 ||
       strcmp(argv[i], "--backup") == 0) {
            const char* nextArg = GetNextArg(argc, argv, i);
            if (nextArg == NULL) {
                fprintf(stderr, "No value passed for -b / --backup option\n");
                exit(1);
            }
            if ((i + 1) < argc) { // Skip next argument if possible so it is not interpreted as a command
                i++;
            }
            else {
                break;
            }
            SetRestoreBackup(nextArg);
       }
        if (strcmp(argv[i], "-n") == 0 ||
        strcmp(argv[i], "--noback") == 0) {
            if (out == COMMANDS_RESTORE || COMMANDS_RESTORE_NO_BACKUP) {
                out = COMMANDS_RESTORE_NO_BACKUP;
            }
        }
    }
    return out;
}