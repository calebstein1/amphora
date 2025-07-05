#include "engine/amphora.h"
#include "colors.h"

#include "../Entities/Player.h"
#include "../Utils/HealthBar.hpp"
#include "../FX/motion_blur.h"
#include "../FX/snowfall.h"

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
Player *player;
AmphoraImage *rotating_heart;
AmphoraEmitter *snow;
AmphoraString *hello;
AmphoraString *timer;
AmphoraString *stationary;
AmphoraString *coords;
enum player_state_e player_state = idle;
enum player_directions_e player_direction = LEFT;
HealthBar *health_bar = nullptr;
static bool blur = false;

Amphora_BeginScene(Level1)

void
Level1_Init() {
	const std::string welcome_message = "Hello, and welcome to the Amphora demo!";
	const std::string message = "I'm going to be fixed right here in place!";

	Amphora_SetSceneFadeParameters(1000, black);
	Amphora_SetBGColor(sky);
	Amphora_SetMap("Overworld", 2);
	Amphora_SetMusic("forest");

	player = new Player(
		static_cast<float>(Amphora_LoadNumber("x", 96)),
		static_cast<float>(Amphora_LoadNumber("y", 148)),
		2.0,
		static_cast<bool>(Amphora_LoadNumber("flip", false))
	);
	rotating_heart = Amphora_CreateSprite("Objects", 128, 72, 3.0, false, false, 1000);

	Amphora_AddFrameset(rotating_heart, "Stationary", nullptr, 64, 129, 16, 16, 0, 0, 1, 0);
	Amphora_AddFrameset(rotating_heart, "Rotate", nullptr, 64, 129, 16, 16, 0, 0, 4, 250);

	snow = Amphora_CreateEmitter(0, 0,
		Amphora_GetMapRectangle()->w,
		Amphora_GetMapRectangle()->h,
		Amphora_GetMapRectangle()->w / 2,
		Amphora_GetMapRectangle()->h / 2,
		(int)Amphora_GetMapRectangle()->w,
		(int)Amphora_GetMapRectangle()->h,
		1024, 4, 4, white,
		false, 10000, Snowfall
	);

	hello = Amphora_CreateString("Roboto", 32, 16, 16, 1000, black, true, "%s", welcome_message.c_str());
	timer = Amphora_CreateString("Merriweather", 32, -16, 16, 1000, black, true, "0");
	stationary = Amphora_CreateString("Merriweather", 16, 76, 132, 1000, black, false, "%s", message.c_str());
	coords = Amphora_CreateString("Roboto", 32, 16, -16, 1000, black, true, "0, 0");

	Amphora_SetCameraTarget(player->PlayerImage);
	Amphora_BoundCameraToMap();

	health_bar = new HealthBar((int) Amphora_LoadNumber("health", 3));

	if (Amphora_GetSessionData("p_state") == ko) {
		Amphora_PlayOneshot(player->PlayerImage, "KO", nullptr);
	} else if (Amphora_GetSessionData("p_state") == atk) {
		player_state = idle;
	}
}

void
Level1_Update(Uint32 frame, const InputState *key_actions) {
	static bool f_down = false;
	static Uint32 damage_cooldown = 0;
	float player_speed = 1;

	Amphora_PlayMusic(500);

	Amphora_TypeString(hello, 60, nullptr);

	Amphora_ObjectClicked(player, SDL_BUTTON_LEFT, [] {
		health_bar->set_health(0);
	});

	if (health_bar->get_health() <= 0 && player_state != ko) {
		player_state = ko;
		Amphora_PlayOneshot(player->PlayerImage, "KO", nullptr);
		if (blur) {
			Amphora_ResetImage(player->PlayerImage);
			blur = false;
		}
	}

	if (key_actions->toggle_fullscreen && !f_down) {
		f_down = true;
		Ampohra_IsWindowFullscreen() ? Amphora_SetWindowWindowed() : Amphora_SetWindowFullscreen();
	} else if (!key_actions->toggle_fullscreen && f_down) {
		f_down = false;
	}

	Amphora_SetFramesetAnimationTime(player->PlayerImage, "Walk", key_actions->dash ? 166 : 250);
	player_speed *= key_actions->dash ? 2 : 1;
	if (key_actions->left && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = LEFT;
		Amphora_SetFrameset(player->PlayerImage, "Walk");
		Amphora_FlipSprite(player->PlayerImage);
		Amphora_MoveSprite(player->PlayerImage, -player_speed, 0);
		Amphora_PlaySFX("leaves01", 1, 0);
		if (!blur) {
			Amphora_ApplyFXToImage(player->PlayerImage, MotionBlur);
			blur = true;
		}
	}
	if (key_actions->right && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = RIGHT;
		Amphora_SetFrameset(player->PlayerImage, "Walk");
		Amphora_UnflipSprite(player->PlayerImage);
		Amphora_MoveSprite(player->PlayerImage, player_speed, 0);
		Amphora_PlaySFX("leaves02", 1, 0);
		if (!blur) {
			Amphora_ApplyFXToImage(player->PlayerImage, MotionBlur);
			blur = true;
		}
	}
	if (key_actions->up && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = UP;
		Amphora_SetFrameset(player->PlayerImage, "Walk");
		Amphora_MoveSprite(player->PlayerImage, 0, -player_speed);
		Amphora_PlaySFX("leaves01", 1, 0);
	}
	if (key_actions->down && player_state != atk && player_state != ko) {
		player_state = walk;
		player_direction = DOWN;
		Amphora_SetFrameset(player->PlayerImage, "Walk");
		Amphora_MoveSprite(player->PlayerImage, 0, player_speed);
		Amphora_PlaySFX("leaves02", 1, 0);
	}
	if (key_actions->attack && player_state != atk && player_state != ko) {
		player_state = atk;
		if (!blur) {
			Amphora_ApplyFXToImage(player->PlayerImage, MotionBlur);
			blur = true;
		}
		Amphora_PlayOneshot(player->PlayerImage, "Attack", [] {
			player_state = idle;
			blur = false;
			Amphora_ResetImage(player->PlayerImage);
			Amphora_SetFrameset(player->PlayerImage, "Idle");
		});
	}
	if (!key_actions->left && !key_actions->right && !key_actions->up && !key_actions->down &&
	    player_state == walk) {
		player_state = idle;
		Amphora_SetFrameset(player->PlayerImage, "Idle");
		if (blur) {
			Amphora_ResetImage(player->PlayerImage);
			blur = false;
		}
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
			Amphora_SetFrameset(player->PlayerImage, "Idle");
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

	Amphora_UpdateStringText(coords, "%.2f, %.2f", Amphora_GetSpritePosition(player->PlayerImage).x,
				 Amphora_GetSpritePosition(player->PlayerImage).y);

	if (frame % Amphora_GetFPS() == 0) {
		Amphora_UpdateStringText(timer, "%d", frame / Amphora_GetFPS());
	}

	if (Amphora_CheckObjectGroupCollision(player->PlayerImage, "Collision")) {
		switch (player_direction) {
			case LEFT:
				Amphora_MoveSprite(player->PlayerImage, player_speed, 0);
				break;
			case RIGHT:
				Amphora_MoveSprite(player->PlayerImage, -player_speed, 0);
				break;
			case UP:
				Amphora_MoveSprite(player->PlayerImage, 0, player_speed);
				break;
			case DOWN:
				Amphora_MoveSprite(player->PlayerImage, 0, -player_speed);
				break;
		}
	}
}

void
Level1_Destroy() {
	Vector2f player_pos = Amphora_GetSpritePosition(player->PlayerImage);
	Amphora_StoreSessionData("p_state", player_state);
	Amphora_SaveNumber("x", player_pos.x);
	Amphora_SaveNumber("y", player_pos.y);
	Amphora_SaveNumber("flip", Amphora_IsSpriteFlipped(player->PlayerImage));
	Amphora_SaveNumber("health", health_bar->get_health());
	player = nullptr;
	rotating_heart = nullptr;
	snow = nullptr;
	hello = nullptr;
	timer = nullptr;
	stationary = nullptr;
	coords = nullptr;
	blur = false;
	Amphora_FadeOutMusic(500);

	delete health_bar;
}
