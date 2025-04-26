#include "engine/internal/error.h"
#include "engine/internal/particles.h"
#include "engine/internal/random.h"
#include "engine/internal/render.h"

/*
 * Prototypes for private functions
 */

SDL_FPoint Amphora_CalculateParticleStartPosition(float start_x, float start_y, int spread_x, int spread_y);

AmphoraEmitter *
Amphora_CreateEmitter(float x, float y, float w, float h, float start_x, float start_y, int spread_x, int spread_y, int count, float p_w, float p_h, SDL_Color color, bool stationary, Sint32 order, void (*update_fn)(AmphoraParticle *, SDL_FRect)) {
	AmphoraEmitter *emitter = NULL;
	struct render_list_node_t *render_list_node = NULL;
	SDL_FPoint position;
	int i;

	if ((emitter = SDL_malloc(sizeof(AmphoraEmitter))) == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to initialize emitter");

		return NULL;
	}
	render_list_node = Amphora_AddRenderListNode(order);

	emitter->type = AMPH_OBJ_EMITTER;
	if (!((emitter->texture = SDL_CreateTexture(Amphora_GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, (int)w, (int)h)))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to create emitter texture");
		render_list_node->garbage = true;
		SDL_free(emitter);

		return NULL;
	}
	emitter->rectangle = (SDL_FRect) { x, y, w, h };
	if (!((emitter->particles = SDL_malloc(count * sizeof(AmphoraParticle))))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate particles");
		render_list_node->garbage = true;
		SDL_DestroyTexture(emitter->texture);
		SDL_free(emitter);

		return NULL;
	}
	emitter->particles_count = count;
	emitter->initial_color = color;
	emitter->start_position = (SDL_FPoint) { start_x, start_y };
	emitter->spread_x = spread_x;
	emitter->spread_y = spread_y;
	emitter->update = update_fn;
	emitter->render_list_node = render_list_node;
	render_list_node->type = AMPH_OBJ_EMITTER;
	render_list_node->data = emitter;
	render_list_node->stationary = stationary;

	for (i = 0; i < count; i++) {
		position = Amphora_CalculateParticleStartPosition(start_x, start_y, spread_x, spread_y);
		emitter->particles[i].x = position.x;
		emitter->particles[i].y = position.y;
		emitter->particles[i].w = p_w;
		emitter->particles[i].h = p_h;
		emitter->particles[i].color = color;
		emitter->particles[i].emitter = emitter;
	}

	return emitter;
}

int
Amphora_DestroyEmitter(AmphoraEmitter *emitter) {
	SDL_DestroyTexture(emitter->texture);
	SDL_free(emitter->particles);
	emitter->render_list_node->garbage = true;
	SDL_free(emitter);
	emitter = NULL;

	return AMPHORA_STATUS_OK;
}

/*
 * Internal functions
 */

void
Amphora_RenderParticleEmitter(AmphoraEmitter *emitter) {
}

/*
 * Private functions
 */

SDL_FPoint
Amphora_CalculateParticleStartPosition(float start_x, float start_y, int spread_x, int spread_y) {
	return (SDL_FPoint) {
		start_x + (float)(Amphora_GetRandom(spread_x) - spread_x / 2.0),
		start_y + (float)(Amphora_GetRandom(spread_y) - spread_y / 2.0),
	};
}
