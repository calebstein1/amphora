#ifndef AMPHORA_DB_H
#define AMPHORA_DB_H

#include "SDL.h"
#include "sqlite3.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Get the open database */
sqlite3 *get_db(void);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_DB_H */
