#ifndef AMPHORA_DB_H
#define AMPHORA_DB_H

#include "SDL.h"

/* Save an integer with key attribute */
int save_number_value(const char *attribute, Sint64 value);
/* Save a string with key attribute */
int save_string_value(const char *attribute, const char *value);
/* Get the integer stored with key attribute, returns default_value if attribute doesn't exist */
Sint64 get_number_value(const char *attribute, Sint64 default_value);
/* Get the string stored with key attribute and store it in out_string */
int get_string_value(const char *attribute, char **out_string);

#endif /* AMPHORA_DB_H */
