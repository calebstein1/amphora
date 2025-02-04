#include "engine/internal/hash_table.h"
#include "engine/internal/lib.h"

/* Prototypes for private functions */
int Amphora_HTProbeForBucket(const HT_HashTable *t, Uint32 hash, int i, int len);
int Amphora_HTProbeForFreeBucket(const HT_HashTable *t, int i, int len);

/*
 * Internal functions
 */

void *
Amphora_HTGetValuePtr(const char *key, const HT_HashTable *t, int len) {
	Uint32 hash = Amphora_crc32(key);
	int i = (int)(hash % len);
	size_t k_len = SDL_strlen(key);

	if (t[i].h == hash && SDL_memcmp(&t[i].b, key, k_len > sizeof(Uint32) ? sizeof(Uint32) : k_len) == 0)
		return (void *)&t[i].d;

	i = Amphora_HTProbeForBucket(t, hash, i, len);

	return i == -1 ? NULL : (void *)&t[i].d;
}

Uint32
Amphora_HTSetValuePtr(const char *key, Uint64 val, size_t nbytes, HT_HashTable *t, int len) {
	Uint32 hash = Amphora_crc32(key);
	int i = (int)(hash % len);
	size_t k_len = SDL_strlen(key);

	if (nbytes > sizeof(Uint64)) return -1;

	if (t[i].h && (t[i].h != hash || SDL_memcmp(&t[i].b, key, k_len > sizeof(Uint32) ? sizeof(Uint32) : k_len) != 0))
		i = Amphora_HTProbeForFreeBucket(t, i, len);
	if (i == -1) return 0;

	t[i].h = hash;
	t[i].d = val;
	SDL_memcpy(&t[i].b, key, k_len > sizeof(Uint32) ? sizeof(Uint32) : k_len);

	return hash;
}

/*
 * Private functions
 */

int
Amphora_HTProbeForBucket(const HT_HashTable *t, Uint32 hash, int i, int len) {
	int s = i ? i - 1 : len - 1;

	while (t[i].h != hash) {
		if (++i == len) i = 0;
		if (i == s) return -1;
	}

	return i;
}

int
Amphora_HTProbeForFreeBucket(const HT_HashTable *t, int i, int len) {
	int s = i ? i - 1 : len - 1;

	while (t[i].h) {
		if (++i == len) i = 0;
		if (i == s) return -1;
	}

	return i;
}
