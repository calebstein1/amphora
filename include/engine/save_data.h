#ifndef AMPHORA_SAVE_DATA_H
#define AMPHORA_SAVE_DATA_H

#include "SDL.h"

int save_number_value(const char *attribute, Sint64 value);
int save_string_value(const char *attribute, const char *value);
Sint64 get_number_value(const char *attribute, Sint64 default_value);

#endif /* AMPHORA_SAVE_DATA_H */
