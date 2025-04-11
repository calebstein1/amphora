#include "engine/amphora.h"
#include "FX/motion_blur.h"

void MotionBlur(SDL_Surface *surface) {
	AmphoraPixelDataABGR *pixels = surface->pixels;
	int i;

	for (i = 0; i < surface->w * surface->h; i++) {
		if (!pixels[i].pixel && pixels[i + 1].pixel) {
			pixels[i + 2].color_data.alpha -= 0x11;
			pixels[i + 1].color_data.alpha -= 0x22;
			pixels[i].color_data.alpha = pixels[i + 1].color_data.alpha - 0x44;
			pixels[i - 1].color_data.alpha = pixels[i + 1].color_data.alpha - 0x88;
			pixels[i - 2].color_data.alpha = pixels[i + 1].color_data.alpha - 0xcc;
		}
	}
}
