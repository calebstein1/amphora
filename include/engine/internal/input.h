#ifndef INPUT_INTERNAL_H
#define INPUT_INTERNAL_H

#define MASK 0xfffffffe
#define MAX_CONTROLLERS 4

#include "engine/internal/render.h"
#include "engine/input.h"

union input_state_u {
	struct input_state_t state; /* Individual flags for the named input actions */
	Uint32 bits; /* The raw input bitfield */
};

enum input_actions {
#define KMAP(action, ...) ACTION_##action,
	DEFAULT_KEYMAP
#undef KMAP
	ACTION_COUNT
};

_Static_assert(ACTION_COUNT <= 32, "Cannot define more than 32 actions");

int Amphora_InitInput(void);
struct input_state_t *Amphora_GetKeyActionState(void);
void Amphora_AddController(Sint32 idx);
void Amphora_RemoveController(SDL_JoystickID id);
void Amphora_ReleaseControllers(void);
void Amphora_HandleKeyDown(const SDL_Event *e);
void Amphora_HandleKeyUp(const SDL_Event *e);
void Amphora_HandleGamepadDown(const SDL_Event *e);
void Amphora_HandleGamepadUp(const SDL_Event *e);
void Amphora_HandleJoystick(const SDL_Event *e);

#endif /* INPUT_INTERNAL_H */
