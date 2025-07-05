#ifndef PLAYER_H
#define PLAYER_H

#include "engine/amphora.h"

class Player {
public:
	AmphoraImage *PlayerImage;

	Player(float x, float y, float scale, bool flip);
};

#endif //PLAYER_H
