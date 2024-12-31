#ifndef INPUT_INTERNAL_H
#define INPUT_INTERNAL_H

#define MASK 0xfffffffffffffffe
#define MAX_CONTROLLERS 4

#include "engine/internal/render.h"
#include "engine/input.h"

union input_state_u {
	struct input_state_t state; /* Individual flags for the named input actions */
	Uint64 bits; /* The raw input bitfield */
};

enum input_actions {
#define KMAP(action, ...) ACTION_##action,
	DEFAULT_KEYMAP
#undef KMAP
	ACTION_COUNT
};

_Static_assert(ACTION_COUNT <= 64, "Cannot define more than 64 actions");

int init_input(void);
struct input_state_t *get_key_actions_state(void);
void add_controller(Sint32 idx);
void remove_controller(SDL_JoystickID id);
void cleanup_controllers(void);
void handle_keydown(const SDL_Event *e);
void handle_keyup(const SDL_Event *e);
void handle_gamepad_down(const SDL_Event *e);
void handle_gamepad_up(const SDL_Event *e);
Vector2f handle_gamepad_joystick(const SDL_Event *e);

#endif /* INPUT_INTERNAL_H */
