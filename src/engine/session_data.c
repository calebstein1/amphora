#include "engine/internal/ht_hash.h"
#include "engine/internal/session_data.h"

/* File-scoped variables */
static HT_HashTable sd;

long
Amphora_GetSessionData(const char *key) {
	long d;

#ifdef DEBUG
	SDL_Log("Read %ld from session data key %s\n", d = HT_GetValue(key, sd), key);
#endif
	return d;
}

void
Amphora_StoreSessionData(const char *key, long val) {
#ifdef DEBUG
	SDL_Log("Storing %ld in session data with key %s\n", val, key);
#endif
	HT_SetValue(key, val, sd);
}

void
Amphora_DeleteSessionData(const char *key) {
#ifdef DEBUG
	SDL_Log("Deleting key %s from session data\n", key);
#endif
	HT_DeleteKey(key, sd);
}

/*
 * Internal functions
 */

void
Amphora_InitSessionData(void) {
	sd = HT_NewTable();
}

void
Amphora_DeInitSessionData(void) {
	HT_FreeTable(sd);
}
