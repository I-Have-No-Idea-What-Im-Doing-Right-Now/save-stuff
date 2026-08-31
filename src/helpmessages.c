//
// Created by Leo Pilcher on 8/31/26.
//
#include <stdio.h>
#include "commands.h"

#include "helpmessages.h"

#define SAVESTUFF_VERSION "1.0.0"

void PrintVersion() {
    printf("%.64s\n", SAVESTUFF_VERSION);
}

void PrintHelp(enum Commands command) {
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