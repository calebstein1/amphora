#ifndef UNTITLED_PLATFORMER_INPUT_H
#define UNTITLED_PLATFORMER_INPUT_H

#include "SDL.h"

#include "config.h"

struct input_state_t {
#define KMAP(action, ...) bool action : 1;
	DEFAULT_KEYMAP
#undef KMAP
};

#ifdef __cplusplus
extern "C" {
#endif
void load_keymap(void);
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_INPUT_H */
