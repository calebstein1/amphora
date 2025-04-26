#include "engine/amphora.h"
#include "FX/snowfall.h"

void
Snowfall(AmphoraParticle *particle, const SDL_FRect *rect) {
	if (!particle->data3) {
		particle->vx = Amphora_GetRandomF() - 0.5f;
		particle->vy = Amphora_GetRandomF();
		particle->data1 = particle->x;
		if (particle->vx > 0) particle->data3 = 1;
		else particle->data3 = -1;
	}

	particle->y += particle->vy;

	if (particle->data3 == 1) {
		particle->x += particle->vx * 0.5f;
		if (particle->x > particle->data1 + 3) particle->data3 = -1;
	} else {
		particle->x -= particle->vx * 0.5f;
		if (particle->x < particle->data1 - 3) particle->data3 = 1;
	}

	if (particle->y > rect->h) particle->y = 0;
}
