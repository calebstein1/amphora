#include "engine/internal/ht_hash.h"
#include "engine/internal/session_data.h"

/* File-scoped variables */
static HT_HashTable sd;

long
Amphora_GetSessionData(const char *key) {
	return (long)HT_GetValue(key, sd);
}

void
Amphora_StoreSessionData(const char *key, long val) {
	HT_SetValue(key, val, sd);
}

void
Amphora_DeleteSessionData(const char *key) {
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
