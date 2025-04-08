#include "engine/amphora.h"
#include "FX/motion_blur.h"

void MotionBlur(SDL_Surface *surface) {
	Uint32 *pixels = (Uint32 *)surface->pixels, i;

	for (i = 0; i < surface->w * surface->h; i++) {
		if (!pixels[i] && pixels[i + 1]) {
			pixels[i + 2] -= 0x11000000;
			pixels[i + 1] -= 0x22000000;
			pixels[i] = pixels[i + 1] - 0x44000000;
			pixels[i - 1] = pixels[i + 1] - 0x88000000;
			pixels[i - 2] = pixels[i + 1] - 0xcc000000;
		}
	}
}
