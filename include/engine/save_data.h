#ifndef AMPHORA_SAVE_DATA_H
#define AMPHORA_SAVE_DATA_H

#include "SDL.h"

int save_number_value(const char *attribute, int value);
int save_string_value(const char *attribute, const char *value);
int get_number_value(const char *attribute, int default_value);

#endif /* AMPHORA_SAVE_DATA_H */
