//
// Created by Leo Pilcher on 8/31/26.
//
#include <stdlib.h>
#include "crossplatlimits.h"
#include "savestuff.h"
#include "cleanup.h"

char TARGET_DIR[SAFE_PATH_MAX] = "../Backups";
char BACKUP_NAME[SAFE_DIR_MAX] = "Backup_%Y-%m-%d_%H-%M-%S";
char BACKUP_TO_RESTORE[SAFE_PATH_MAX] = "";
char **IGNORES = NULL;
size_t IGNORES_LEN = 0;

void Cleanup() {
    free(IGNORES);
}