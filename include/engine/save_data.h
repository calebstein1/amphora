#ifndef AMPHORA_SAVE_DATA_H
#define AMPHORA_SAVE_DATA_H

#ifdef __cplusplus
extern "C" {
#endif
/* Save an integer with keys attribute */
int save_number_value(const char *attribute, Sint64 value);
/* Save a string with keys attribute */
int save_string_value(const char *attribute, const char *value);
/* Get the integer stored with keys attribute, returns default_value if attribute doesn't exist */
Sint64 get_number_value(const char *attribute, Sint64 default_value);
/* Get the string stored with keys attribute and store it in out_string */
int get_string_value(const char *attribute, char **out_string);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_SAVE_DATA_H */
