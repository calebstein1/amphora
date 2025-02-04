#ifndef AMPHORA_HASH_TABLE_INTERNAL_H
#define AMPHORA_HASH_TABLE_INTERNAL_H

#include "SDL.h"

#define Amphora_HTZeroTable(t) SDL_memset((t), 0, sizeof((t)))
#define Amphora_HTGetValue(k, type, t) *(type *)Amphora_HTGetValuePtr((k), (t), (sizeof((t)) / sizeof(*(t))))
#define Amphora_HTSetValue(k, type, v, t) Amphora_HTSetValuePtr((k), (uintptr_t)(v), sizeof(type), (t), (sizeof((t)) / sizeof(*(t))))

typedef struct {
    Uint64 d;
    Uint32 h;
    Uint32 b;
} HT_HashTable;

#endif //AMPHORA_HASH_TABLE_INTERNAL_H
