#ifndef PARTICLES_INTERNAL_H
#define PARTICLES_INTERNAL_H

#include "SDL.h"

#include "engine/particles.h"
#include "engine/internal/render.h"

struct emitter_t {
	enum amphora_object_type_e type;
	SDL_Texture *texture;
	SDL_FRect rectangle;
	AmphoraParticle *particles;
	int particles_count;
	SDL_Color initial_color;
	SDL_FPoint start_position;
	int spread_x, spread_y;
	void (*update)(AmphoraParticle *, const SDL_FRect *);
	struct render_list_node_t *render_list_node;
};

void Amphora_UpdateAndRenderParticleEmitter(AmphoraEmitter *emitter);

#endif /* PARTICLES_INTERNAL_H */
