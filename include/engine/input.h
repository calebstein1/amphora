#ifndef UNTITLED_PLATFORMER_INPUT_H
#define UNTITLED_PLATFORMER_INPUT_H

#include "SDL.h"

#include "engine/img.h"

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
/* Execute a callback function if a specified AmphoraImage or AmphoraString is clicked, button is an SDL_BUTTON_X macro */
bool object_clicked(void *spr, int button, void (*callback)(void));
/* Returns true if the mouse is over the provided object, false if not */
bool object_mouseover(void *obj);
/* Gets the currently pressed key */
SDL_Keycode get_pressed_key(void);
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_INPUT_H */
