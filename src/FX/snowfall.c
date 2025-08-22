#include "engine/amphora.h"
#include "snowfall.h"

void
Snowfall(int i, int c, AmphoraParticle *p, AmphoraParticleExt *d, const SDL_FRect *rect) {
	if (!d[i].data3) {
		p[i].vx = Amphora_GetRandomF() - 0.5f;
		p[i].vy = Amphora_GetRandomF();
		d[i].data1 = p[i].x;
		d[i].data3 = p[i].vx > 0 ? 1 : 0;
	}

	p[i].y += p[i].vy;
	p[i].color.a = (Uint8)(255.0f * (1.0f - p[i].y / rect->h));
	if (p[i].color.a < 128) p[i].color.a = 128;

	if (d[i].data3 == 1) {
		p[i].x += p[i].vx * 0.5f;
		if (p[i].x > d[i].data1 + 3) d[i].data3 = -1;
	} else {
		p[i].x -= p[i].vx * 0.5f;
		if (p[i].x < d[i].data1 - 3) d[i].data3 = 1;
	}

	if (p[i].y > rect->h) p[i].y = 0;
}
