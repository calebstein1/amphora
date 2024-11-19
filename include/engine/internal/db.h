#ifndef AMPHORA_SAVE_DATA_INTERNAL_H
#define AMPHORA_SAVE_DATA_INTERNAL_H

#include "engine/db.h"

int init_db(void);
void cleanup_db(void);
void get_key_map_or_default(const char **actions, SDL_Keycode *keys, SDL_GameControllerButton *gamepad);

#endif /* AMPHORA_SAVE_DATA_INTERNAL_H */
