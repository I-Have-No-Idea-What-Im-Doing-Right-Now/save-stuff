//
// Created by Leo Pilcher on 8/22/26.
//

#include <stdio.h>
#include "commands.h"
#include "ParseArgs.h"
#include "backups.h"
#include "cleanup.h"
#include "helpmessages.h"

int main(const int argc, char *argv[]) {
    const enum Commands command = ParseArgs(argc, argv);
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
            MakeBackup();
            RestoreBackup();
            break;
        case COMMANDS_RESTORE_NO_BACKUP:
            PrintWarn("WARNING: Restoring without a backup is not recommended!\n", 56);
            RestoreBackup();
        default:
            break;
    }
    Cleanup();
    return 0;
}