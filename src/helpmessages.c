//
// Created by Leo Pilcher on 8/31/26.
//
#include <stdio.h>
#include "version.h"
#include "commands.h"
#include "helpmessages.h"
#include <stdlib.h>

void PrintVersion() {
    printf("%.64s\n", SAVESTUFF_VERSION);
}

void PrintHelp(enum Commands command) {
#include <generated/generalhelp.h>
#include <generated/versionhelp.h>
#include <generated/backuphelp.h>
#include <generated/restorehelp.h>
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
            fwrite(restore_txt, sizeof(char), restore_txt_len, stdout);
            break;
        default:
            return;
    }
    putchar('\n');
}

void PrintWarn(char *message, const size_t len) {
    const char *yellow = "\033[93m";
    const char *reset = "\033[0m";
    const size_t strSize = len + 15; // Add 7 for each ANSI escape and 1 for null terminator
    char *finalMessage = malloc(strSize);
    snprintf(finalMessage, strSize, "%s%s%s", yellow, message, reset);
    printf("%s", finalMessage);
    free(finalMessage);
}
