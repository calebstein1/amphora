#include "engine/internal/error.h"
#include "engine/internal/events.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"

/*
 * Internal functions
 */

Uint32
Amphora_ProcessEventLoop(SDL_Event *e) {
	while (SDL_PollEvent(e)) {
		switch (e->type) {
			case SDL_QUIT:
				return e->type;
			case SDL_KEYDOWN:
				Amphora_HandleKeyDown(e);
				break;
			case SDL_KEYUP:
				Amphora_HandleKeyUp(e);
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				Amphora_HandleGamepadDown(e);
				break;
			case SDL_CONTROLLERBUTTONUP:
				Amphora_HandleGamepadUp(e);
				break;
			case SDL_CONTROLLERAXISMOTION:
				Amphora_HandleJoystick(e);
				break;
			case SDL_CONTROLLERDEVICEADDED:
				Amphora_AddController(e->cdevice.which);
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				Amphora_RemoveController(e->cdevice.which);
				break;
			case SDL_WINDOWEVENT:
				if (e->window.event != SDL_WINDOWEVENT_RESIZED) break;

				Amphora_SetRenderLogicalSize(Amphora_GetResolution());
		}
	}

	return AMPHORA_STATUS_OK;
}
