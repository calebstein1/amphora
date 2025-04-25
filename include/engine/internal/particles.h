#ifndef PARTICLES_INTERNAL_H
#define PARTICLES_INTERNAL_H

#include "SDL.h"

#include "engine/particles.h"
#include "engine/internal/render.h"

struct emitter_t {
	SDL_Texture *texture;
	SDL_FRect rectangle;
	AmphoraParticle *particles;
	int particles_count;
	SDL_Color color;
	SDL_FPoint start_position;
	int start_spread_x, start_spread_y;
	void (*update)(AmphoraParticle *, SDL_FRect);
	struct render_list_node_t *render_list_node;
};

void Amphora_RenderParticleEmitter(AmphoraEmitter *emitter);

#endif /* PARTICLES_INTERNAL_H */
