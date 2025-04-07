#include "engine/amphora.h"
#include "FX/motion_blur.h"

static Uint32 modPixels[0x10000];

void MotionBlur(SDL_Surface *surface) {
	Uint32 *pixels = (Uint32 *)surface->pixels, i;
	Uint32 *pModPixels = modPixels;

	for (i = 0; i < surface->w * surface->h; i++) {
		if (!pixels[i] && pixels[i + 1]) {
			*pModPixels++ = i + 2;
			*pModPixels++ = i + 1;
			*pModPixels++ = i;
			*pModPixels++ = i - 1;
			*pModPixels++ = i - 2;

			pixels[i + 2] -= 0x11000000;
			pixels[i + 1] -= 0x22000000;
			pixels[i] = pixels[i + 1] - 0x44000000;
			pixels[i - 1] = pixels[i + 1] - 0x88000000;
			pixels[i - 2] = pixels[i + 1] - 0xcc000000;
		}
	}
}

void ResetMotionBlur(SDL_Surface *surface) {
	Uint32 *pixels = (Uint32 *)surface->pixels, i = 0, c = 0;

	while (modPixels[i]) {
		if (!c) {
			pixels[modPixels[i]] += 0x11000000;
		} else if (c == 1) {
			pixels[modPixels[i]] += 0x22000000;
		} else {
			pixels[modPixels[i]] = 0;
		}
		if (++c == 5) c = 0;
		i++;
	}
	SDL_memset(modPixels, 0, sizeof(modPixels));
}
