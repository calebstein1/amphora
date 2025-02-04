#include "engine/internal/hash_table.h"
#include "engine/internal/lib.h"
#include "engine/util.h"

/* Prototypes for private functions */
static int Amphora_HTProbeForBucket(const HT_HashTable *t, Uint32 hash, int i, int len, bool set);

/*
 * Internal functions
 */

void *
Amphora_HTGetValuePtr(const char *key, const HT_HashTable *t, int len) {
	const Uint32 hash = Amphora_crc32(key);
	int i = (int)(hash % len);
	const size_t k_len = SDL_strlen(key);

	if (t[i].h == hash && SDL_memcmp(&t[i].b, key, k_len > sizeof(Uint32) ? sizeof(Uint32) : k_len) == 0)
		return (void *)&t[i].d;

	i = Amphora_HTProbeForBucket(t, hash, i, len, false);

	return i == -1 ? NULL : (void *)&t[i].d;
}

Uint32
Amphora_HTSetValuePtr(const char *key, Uint64 val, size_t nbytes, HT_HashTable *t, int len) {
	const Uint32 hash = Amphora_crc32(key);
	int i = (int)(hash % len);
	const size_t k_len = SDL_strlen(key);

	if (nbytes > sizeof(Uint64)) return 1;

	if (t[i].h && (t[i].h != hash || SDL_memcmp(&t[i].b, key, k_len > sizeof(Uint32) ? sizeof(Uint32) : k_len) != 0))
		i = Amphora_HTProbeForBucket(t, hash, i, len, true);
	if (i == -1) return 1;

	t[i].h = hash;
	t[i].d = val;
	SDL_memcpy(&t[i].b, key, k_len > sizeof(Uint32) ? sizeof(Uint32) : k_len);

	return hash;
}

/*
 * Private functions
 */

static int
Amphora_HTProbeForBucket(const HT_HashTable *t, Uint32 hash, int i, int len, bool set) {
	const int s = i ? i - 1 : len - 1;

	while (t[i].h != hash) {
		if (set && !t[i].d) break;
		if (i == s) return -1;
		if (++i == len) i = 0;
	}

	return i;
}
