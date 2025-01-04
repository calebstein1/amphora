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
/* Change the keymap for an action */
void update_keymap(const char *action, SDL_Keycode keycode);
/* Execute a callback function if a specified AmphoraImage or AmphoraString is clicked, button is an SDL_BUTTON_X macro */
bool object_clicked(void *spr, int button, void (*callback)(void));
/* Returns true if the mouse is over the provided object, false if not */
bool object_mouseover(void *obj);
/* Gets the currently pressed key */
SDL_Keycode get_pressed_key(void);
/* Get the name of the key associated with an action */
char *get_action_key_name(const char *action);
/* Run a callback function for each defined action name */
void for_each_action(void (*callback)(const char *));
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_INPUT_H */
