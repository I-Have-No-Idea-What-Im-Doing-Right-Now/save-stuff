//
// Created by Leo Pilcher on 8/31/26.
//

#ifndef SAVESTUFF_SAVESTUFF_H
#define SAVESTUFF_SAVESTUFF_H

#include <stdlib.h>
#include "crossplatlimits.h"

extern char TARGET_DIR[SAFE_PATH_MAX];
extern char BACKUP_NAME[SAFE_DIR_MAX];
extern char BACKUP_TO_RESTORE[SAFE_PATH_MAX];
extern char **IGNORES;
extern size_t IGNORES_LEN;

#endif //SAVESTUFF_SAVESTUFF_H
