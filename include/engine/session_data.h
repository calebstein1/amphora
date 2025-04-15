#ifndef AMPHORA_SESSION_DATA_H
#define AMPHORA_SESSION_DATA_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Get the value associated with a key from session data */
long Amphora_GetSessionData(const char *key);
/* Store a value associated with a key to session data */
void Amphora_StoreSessionData(const char *key, long val);
/* Delete a key from session data */
void Amphora_DeleteSessionData(const char *key);
#ifdef __cplusplus
}
#endif

#endif /* AMPHORA_SESSION_DATA_H */
