#ifndef AMPHORA_DB_H
#define AMPHORA_DB_H

#include "SDL.h"
#include "sqlite3.h"

/* Get the open database */
sqlite3 *get_db(void);

#endif /* AMPHORA_DB_H */
