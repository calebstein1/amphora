#ifndef PARTICLES_H
#define PARTICLES_H

#include "render.h"

typedef struct {
	float x, y, w, h, vx, vy;
	SDL_Color color;
	void *data;
	AmphoraEmitter *emitter;
} AmphoraParticle;

#ifdef __cplusplus
extern "C" {
#endif
AmphoraEmitter *Amphora_CreateEmitter(float x, float y, float w, float h, float start_x, float start_y, int spread_x, int spread_y, int count, float p_w, float p_h, SDL_Color color, bool stationary, Sint32 order, void (*update_fn)(AmphoraParticle *, SDL_FRect));
int Amphora_DestroyEmitter(AmphoraEmitter *emitter);
#ifdef __cplusplus
}
#endif
#endif /* PARTICLES_H */
