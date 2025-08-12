#include "engine/internal/error.h"
#include "engine/internal/memory.h"
#include "engine/internal/particles.h"
#include "engine/internal/random.h"
#include "engine/internal/render.h"

/*
 * Prototypes for private functions
 */

SDL_FPoint Amphora_CalculateParticleStartPosition(float start_x, float start_y, int spread_x, int spread_y);

AmphoraEmitter *
Amphora_CreateEmitter(float x, float y, float w, float h, float start_x, float start_y, int spread_x, int spread_y, int count, float p_w, float p_h, SDL_Color color, bool stationary, Sint32 order, void (*update_fn)(AmphoraParticle *, const SDL_FRect *)) {
	AmphoraEmitter *emitter = NULL;
	struct render_list_node_t *render_list_node = NULL;
	SDL_FPoint position;
	int i;

	if ((emitter = Amphora_HeapAlloc(sizeof(AmphoraEmitter), MEM_EMITTER)) == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to initialize emitter");

		return NULL;
	}
	render_list_node = Amphora_AddRenderListNode(order);

	emitter->type = AMPH_OBJ_EMITTER;
	if (!((emitter->texture = SDL_CreateTexture(Amphora_GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, (int)w, (int)h)))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to create emitter texture");
		render_list_node->garbage = true;
		Amphora_HeapFree(emitter);

		return NULL;
	}
	emitter->rectangle = (SDL_FRect) { x, y, w, h };
	if (!((emitter->particles = Amphora_HeapAlloc(count * sizeof(AmphoraParticle), MEM_EMITTER)))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate particles");
		render_list_node->garbage = true;
		SDL_DestroyTexture(emitter->texture);
		Amphora_HeapFree(emitter);

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

	(void)SDL_SetTextureBlendMode(emitter->texture, SDL_BLENDMODE_BLEND);

	for (i = 0; i < count; i++) {
		position = Amphora_CalculateParticleStartPosition(start_x, start_y, spread_x, spread_y);
		emitter->particles[i].x = position.x;
		emitter->particles[i].y = position.y;
		emitter->particles[i].w = p_w;
		emitter->particles[i].h = p_h;
		emitter->particles[i].color = color;
		emitter->particles[i].data1 = 0;
		emitter->particles[i].data2 = 0;
		emitter->particles[i].data3 = 0;
		emitter->particles[i].data4 = 0;
		emitter->particles[i].hidden = false;
	}

	return emitter;
}

int
Amphora_DestroyEmitter(AmphoraEmitter *emitter) {
	if (!emitter) return AMPHORA_STATUS_FAIL_UNDEFINED;

	SDL_DestroyTexture(emitter->texture);
	Amphora_HeapFree(emitter->particles);
	emitter->render_list_node->garbage = true;
	Amphora_HeapFree(emitter);

	return AMPHORA_STATUS_OK;
}

/*
 * Internal functions
 */

void
Amphora_UpdateAndRenderParticleEmitter(AmphoraEmitter *emitter) {
	SDL_Renderer *renderer = Amphora_GetRenderer();
	SDL_Color color = { 0, 0, 0, 0 };
	SDL_FRect dst;
	Camera camera = Amphora_GetCamera();
	int i;

	(void)SDL_SetRenderTarget(renderer, emitter->texture);
	(void)SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	(void)SDL_RenderClear(renderer);

	for (i = 0; i < emitter->particles_count; i++) {
		if (emitter->update) emitter->update(&emitter->particles[i], &emitter->rectangle);
		if (emitter->particles[i].hidden) continue;
		if (SDL_memcmp(&color, &emitter->particles[i].color, sizeof(SDL_Color)) != 0) {
			(void)SDL_memcpy(&color, &emitter->particles[i].color, sizeof(SDL_Color));
			(void)SDL_SetRenderDrawColor(
				renderer,
				color.r,
				color.g,
				color.b,
				color.a
			);
		}
		dst = (SDL_FRect) {
			emitter->particles[i].x,
			emitter->particles[i].y,
			emitter->particles[i].w,
			emitter->particles[i].h
		};
		if (!emitter->render_list_node->stationary) {
			dst.x -= camera.x;
			dst.y -= camera.y;
		}
		(void)SDL_RenderFillRectF(renderer, &dst);
	}
	(void)SDL_SetRenderTarget(renderer, NULL);
	Amphora_RenderTexture(emitter->texture, NULL, &emitter->rectangle, 0, SDL_FLIP_NONE);
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
