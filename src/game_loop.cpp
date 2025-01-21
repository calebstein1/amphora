#include "engine/amphora.h"
#include "colors.h"

#include <sstream>
#include <string>
#include <vector>

#define DEFAULT_HEALTH 3

class HealthBar {
private:
	std::vector<AmphoraImage *> health_bar;
	int current_health;
public:
	HealthBar() {
		current_health = 0;
	}

	void increase_health() {
		current_health++;
		health_bar.push_back(nullptr);
		Amphora_CreateSprite(&health_bar.back(), "Objects", -96.0f - (float) (32 * (health_bar.size() - 1)), 24,
				     2, false, true, 1000);
		Amphora_AddFrameset(health_bar.back(), "Default", nullptr, 63, 0, 16, 16, 0, 0, 1, 0);
	}

	void decrease_health() {
		if (current_health == 0) return;

		current_health--;
		Amphora_FreeSprite(&health_bar.back());
		health_bar.pop_back();
	}

	[[nodiscard]] int get_health() const {
		return current_health;
	}
};

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
HealthBar health_bar;
AmphoraString *hello;
AmphoraString *timer;
AmphoraString *stationary;
AmphoraString *coords;
enum player_state_e player_state = idle;
enum player_directions_e player_direction = LEFT;

void
Amphora_GameInit() {
	int i;
	const std::string welcome_message = "Hello, and welcome to the Amphora demo!";
	const std::string message = "I'm going to be fixed right here in place!";

	Amphora_SetBGColor(sky);
	Amphora_SetMap("Overworld", 2);
	Amphora_SetMusic("forest");

	Amphora_CreateSprite(&player, "Character", (float) Amphora_LoadNumber("x", 96),
			     (float) Amphora_LoadNumber("y", 148), 2.0, (bool) Amphora_LoadNumber("flip", false), false,
			     101);
	Amphora_CreateSprite(&rotating_heart, "Objects", 128, 72, 3.0, false, false, 1000);
	for (i = 0; i < (int) Amphora_LoadNumber("health", DEFAULT_HEALTH); i++) {
		health_bar.increase_health();
	}

	Amphora_AddFrameset(player, "Idle", nullptr, 0, 17, 32, 48, 0, 0, 1, 0);
	Amphora_AddFrameset(player, "Walk", nullptr, 32, 17, 32, 48, 0, 0, 6, 250);
	Amphora_AddFrameset(player, "Attack", nullptr, 223, 145, 32, 48, 0, 0, 2, 250);
	Amphora_AddFrameset(player, "KO", nullptr, 81, 355, 48, 32, 8, -8, 2, 500);

	Amphora_AddFrameset(rotating_heart, "Stationary", nullptr, 64, 129, 16, 16, 0, 0, 1, 0);
	Amphora_AddFrameset(rotating_heart, "Rotate", nullptr, 64, 129, 16, 16, 0, 0, 4, 250);

	Amphora_CreateString(&hello, "Roboto", 32, 16, 16, 1000, black, welcome_message.c_str(), true);
	Amphora_CreateString(&timer, "Merriweather", 32, -16, 16, 1000, black, "0", true);
	Amphora_CreateString(&stationary, "Merriweather", 16, 76, 132, 1000, black, message.c_str(), false);
	Amphora_CreateString(&coords, "Roboto", 32, 16, -16, 1000, black, "0, 0", true);

	Amphora_SetCameraTarget(player);
}

void
Amphora_GameLoop(Uint64 frame, const struct input_state_t *key_actions) {
	static bool f_down = false;
	static Uint8 hello_ticker = 0;
	static Uint64 damage_cooldown = 0;
	std::stringstream timer_stream;
	std::stringstream coords_stream;
	float player_speed = 1;

	Amphora_PlayMusic(500);

	if (health_bar.get_health() <= 0 && player_state != ko) {
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
	if (!key_actions->left && !key_actions->right && !key_actions->up && !key_actions->down && player_state == walk) {
		player_state = idle;
		Amphora_SetFrameset(player, "Idle");
	}
	if (key_actions->damage && frame - damage_cooldown > (Amphora_GetFPS() / 2)) {
		damage_cooldown = frame;
		health_bar.decrease_health();
	}
	if (key_actions->heal && frame - damage_cooldown > (Amphora_GetFPS() / 2)) {
		damage_cooldown = frame;
		health_bar.increase_health();
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

	if (Amphora_ObjectHover(rotating_heart)) {
		Amphora_SetFrameset(rotating_heart, "Rotate");
	} else {
		Amphora_SetFrameset(rotating_heart, "Stationary");
	}

	coords_stream << Amphora_GetSpritePosition(player).x << ", " << Amphora_GetSpritePosition(player).y;
	Amphora_UpdateStringText(&coords, coords_stream.str().c_str());

	if (frame % Amphora_GetFPS() == 0) {
		timer_stream << frame / Amphora_GetFPS();
		Amphora_UpdateStringText(&timer, timer_stream.str().c_str());
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
Amphora_GameShutdown() {
	Vector2f player_pos = Amphora_GetSpritePosition(player);
	Amphora_SaveNumber("x", player_pos.x);
	Amphora_SaveNumber("y", player_pos.y);
	Amphora_SaveNumber("flip", Amphora_IsSpriteFlipped(player));
	Amphora_SaveNumber("health", health_bar.get_health());
}
