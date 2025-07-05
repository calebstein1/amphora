#include "engine/amphora.h"
#include "snowfall.h"

void
Snowfall(AmphoraParticle *particle, const SDL_FRect *rect) {
	if (!particle->data3) {
		particle->vx = Amphora_GetRandomF() - 0.5f;
		particle->vy = Amphora_GetRandomF();
		particle->data1 = particle->x;
		particle->data3 = particle->vx > 0 ? 1 : 0;
	}

	particle->y += particle->vy;
	particle->color.a = (Uint8)(255.0f * (1.0f - particle->y / rect->h));
	if (particle->color.a < 128) particle->color.a = 128;

	if (particle->data3 == 1) {
		particle->x += particle->vx * 0.5f;
		if (particle->x > particle->data1 + 3) particle->data3 = -1;
	} else {
		particle->x -= particle->vx * 0.5f;
		if (particle->x < particle->data1 - 3) particle->data3 = 1;
	}

	if (particle->y > rect->h) particle->y = 0;
}
