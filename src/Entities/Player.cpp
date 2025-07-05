#include "engine/amphora.h"
#include "Player.h"

Player::Player(float x, float y, float scale, bool flip) {
	PlayerImage = Amphora_CreateSprite("Character", x, y, scale, flip, false, 101);

	Amphora_AddFrameset(PlayerImage, "Idle", nullptr, 0, 17, 32, 48, 0, 0, 1, 0);
	Amphora_AddFrameset(PlayerImage, "Walk", nullptr, 32, 17, 32, 48, 0, 0, 6, 250);
	Amphora_AddFrameset(PlayerImage, "Attack", nullptr, 223, 145, 32, 48, 0, 0, 2, 250);
	Amphora_AddFrameset(PlayerImage, "KO", nullptr, 81, 355, 48, 32, 8, -8, 2, 500);
}
