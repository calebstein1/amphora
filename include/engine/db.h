#ifndef AMPHORA_DB_H
#define AMPHORA_DB_H

#include "SDL.h"
#include "sqlite3.h"

/* Get the open database */
sqlite3 *get_db(void);
/* Save an integer with keys attribute */
int save_number_value(const char *attribute, Sint64 value);
/* Save a string with keys attribute */
int save_string_value(const char *attribute, const char *value);
/* Get the integer stored with keys attribute, returns default_value if attribute doesn't exist */
Sint64 get_number_value(const char *attribute, Sint64 default_value);
/* Get the string stored with keys attribute and store it in out_string */
int get_string_value(const char *attribute, char **out_string);

#endif /* AMPHORA_DB_H */
