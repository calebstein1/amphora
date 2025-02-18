#include "engine/amphora.h"
#include "colors.h"

#include "../Utils/HealthBar.hpp"

#include <string>

enum player_state_e {
	idle,
	walk,
	atk,
	ko
};

enum player_directions_e {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

/* Game globals */
AmphoraImage *player;
AmphoraImage *rotating_heart;
AmphoraString *hello;
AmphoraString *timer;
AmphoraString *stationary;
AmphoraString *coords;
enum player_state_e player_state = idle;
enum player_directions_e player_direction = LEFT;
HealthBar *health_bar = nullptr;

Amphora_BeginScene(Level1)

void
Level1_Init() {
	const std::string welcome_message = "Hello, and welcome to the Amphora demo!";
	const std::string message = "I'm going to be fixed right here in place!";

	Amphora_SetBGColor(sky);
	Amphora_SetMap("Overworld", 2);
	Amphora_SetMusic("forest");

	player = Amphora_CreateSprite("Character", (float) Amphora_LoadNumber("x", 96),
			     (float) Amphora_LoadNumber("y", 148), 2.0, (bool) Amphora_LoadNumber("flip", false), false,
			     101);
	rotating_heart = Amphora_CreateSprite("Objects", 128, 72, 3.0, false, false, 1000);

	Amphora_AddFrameset(player, "Idle", nullptr, 0, 17, 32, 48, 0, 0, 1, 0);
	Amphora_AddFrameset(player, "Walk", nullptr, 32, 17, 32, 48, 0, 0, 6, 250);
	Amphora_AddFrameset(player, "Attack", nullptr, 223, 145, 32, 48, 0, 0, 2, 250);
	Amphora_AddFrameset(player, "KO", nullptr, 81, 355, 48, 32, 8, -8, 2, 500);

	Amphora_AddFrameset(rotating_heart, "Stationary", nullptr, 64, 129, 16, 16, 0, 0, 1, 0);
	Amphora_AddFrameset(rotating_heart, "Rotate", nullptr, 64, 129, 16, 16, 0, 0, 4, 250);

	hello = Amphora_CreateString("Roboto", 32, 16, 16, 1000, black, welcome_message.c_str(), true);
	timer = Amphora_CreateString("Merriweather", 32, -16, 16, 1000, black, "0", true);
	stationary = Amphora_CreateString("Merriweather", 16, 76, 132, 1000, black, message.c_str(), false);
	coords = Amphora_CreateString("Roboto", 32, 16, -16, 1000, black, "0, 0", true);

	Amphora_SetCameraTarget(player);

	health_bar = new HealthBar((int) Amphora_LoadNumber("health", 3));
}

void
Level1_Update(Uint64 frame, const InputState *key_actions) {
	static bool f_down = false;
	static Uint8 hello_ticker = 0;
	static Uint64 damage_cooldown = 0;
	float player_speed = 1;

	Amphora_PlayMusic(500);

	if (health_bar->get_health() <= 0 && player_state != ko) {
		player_state = ko;
		Amphora_PlayOneshot(player, "KO", nullptr);
	}

	if (key_actions->toggle_fullscreen && !f_down) {
		f_down = true;
		Ampohra_IsWindowFullscreen() ? Amphora_SetWindowWindowed() : Amphora_SetWindowFullscreen();
	} else if (!key_actions->toggle_fullscreen && f_down) {
		f_down = false;
	}

	Amphora_SetFramesetAnimationTime(player, "Walk", key_actions->dash ? 166 : 250);
	player_speed *= key_actions->dash ? 2 : 1;
	if (key_actions->left && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = LEFT;
		Amphora_SetFrameset(player, "Walk");
		Amphora_FlipSprite(player);
		Amphora_MoveSprite(player, -player_speed, 0);
		Amphora_PlaySFX("leaves01", 1, 0);
	}
	if (key_actions->right && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = RIGHT;
		Amphora_SetFrameset(player, "Walk");
		Amphora_UnflipSprite(player);
		Amphora_MoveSprite(player, player_speed, 0);
		Amphora_PlaySFX("leaves02", 1, 0);
	}
	if (key_actions->up && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = UP;
		Amphora_SetFrameset(player, "Walk");
		Amphora_MoveSprite(player, 0, -player_speed);
		Amphora_PlaySFX("leaves01", 1, 0);
	}
	if (key_actions->down && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = DOWN;
		Amphora_SetFrameset(player, "Walk");
		Amphora_MoveSprite(player, 0, player_speed);
		Amphora_PlaySFX("leaves02", 1, 0);
	}
	if (key_actions->attack && player_state != atk && player_state != ko) {
		player_state = atk;
		Amphora_PlayOneshot(player, "Attack", [] {
		    player_state = idle;
		    Amphora_SetFrameset(player, "Idle");
		});
	}
	if (!key_actions->left && !key_actions->right && !key_actions->up && !key_actions->down &&
	    player_state == walk) {
		player_state = idle;
		Amphora_SetFrameset(player, "Idle");
	}
	if (key_actions->damage && frame - damage_cooldown > (Amphora_GetFPS() / 2)) {
		damage_cooldown = frame;
		health_bar->decrease_health();
	}
	if (key_actions->heal && frame - damage_cooldown > (Amphora_GetFPS() / 2)) {
		damage_cooldown = frame;
		health_bar->increase_health();
		if (player_state == ko) {
			player_state = idle;
			Amphora_SetFrameset(player, "Idle");
		}
	}
	if (key_actions->zoom) {
		Amphora_SetCameraZoom(150, 60);
	} else if (key_actions->expand) {
		Amphora_SetCameraZoom(75, 60);
	} else {
		Amphora_ResetCameraZoom(60);
	}
	if (key_actions->quit) {
		Amphora_QuitGame();
	}
	if (key_actions->scene2) {
		Amphora_LoadScene("Level2");
	}

	if (Amphora_ObjectHover(rotating_heart)) {
		Amphora_SetFrameset(rotating_heart, "Rotate");
	} else {
		Amphora_SetFrameset(rotating_heart, "Stationary");
	}

	Amphora_UpdateStringText(&coords, "%.2f, %.2f", Amphora_GetSpritePosition(player).x,
				 Amphora_GetSpritePosition(player).y);

	if (frame % Amphora_GetFPS() == 0) {
		Amphora_UpdateStringText(&timer, "%d", frame / Amphora_GetFPS());
	}

	if (hello && Amphora_IsEven(frame) && hello_ticker < Amphora_GetStringLength(hello)) {
		Amphora_UpdateStringCharsDisplayed(&hello, ++hello_ticker);
	}

	if (Amphora_CheckObjectGroupCollision(player, "Collision")) {
		switch (player_direction) {
			case LEFT:
				Amphora_MoveSprite(player, player_speed, 0);
				break;
			case RIGHT:
				Amphora_MoveSprite(player, -player_speed, 0);
				break;
			case UP:
				Amphora_MoveSprite(player, 0, player_speed);
				break;
			case DOWN:
				Amphora_MoveSprite(player, 0, -player_speed);
				break;
		}
	}
}

void
Level1_Destroy() {
	Vector2f player_pos = Amphora_GetSpritePosition(player);
	Amphora_SaveNumber("x", player_pos.x);
	Amphora_SaveNumber("y", player_pos.y);
	Amphora_SaveNumber("flip", Amphora_IsSpriteFlipped(player));
	Amphora_SaveNumber("health", health_bar->get_health());
	player = nullptr;
	rotating_heart = nullptr;
	hello = nullptr;
	timer = nullptr;
	stationary = nullptr;
	coords = nullptr;

	delete (health_bar);
}
