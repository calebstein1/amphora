#ifndef UNTITLED_PLATFORMER_INPUT_H
#define UNTITLED_PLATFORMER_INPUT_H

#include "SDL.h"

#include "engine/img.h"

#include "config.h"

typedef struct input_state_t {
#define KMAP(action, ...) bool action : 1;
	DEFAULT_KEYMAP
#undef KMAP
} InputState;

#ifdef __cplusplus
extern "C" {
#endif
void Amphora_LoadKeymap(void);
/* Change the keymap for an action */
void Amphora_UpdateKeymap(const char *action, SDL_Keycode keycode);
/* Execute a callback function if a specified AmphoraImage or AmphoraString is clicked, button is an SDL_BUTTON_X macro */
bool Amphora_ObjectClicked(void *spr, int button, void (*callback)(void));
/* Returns true if the mouse is over the provided object, false if not */
bool Amphora_ObjectHover(void *obj);
/* Gets the currently pressed key */
SDL_Keycode Amphora_GetPressedKey(void);
/* Returns whether the left joystick is currently in use */
bool Amphora_LeftJoystickActive(void);
/* Returns whether the right joystick is currently in use */
bool Amphora_RightJoystickActive(void);
/* Get the state of the left joystick */
Vector2f Amphora_GetLeftJoystickState(void);
/* Get the state of the left joystick */
Vector2f Amphora_GetRightJoystickState(void);
/* Get the name of the key associated with an action */
char *Amphora_GetActionKeyName(const char *action);
/* Run a callback function for each defined action name */
void Amphora_ForEachAction(void (*callback)(const char *, int));
#ifdef __cplusplus
}
#endif

#endif /* UNTITLED_PLATFORMER_INPUT_H */
