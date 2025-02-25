#include "engine/amphora.h"

Amphora_BeginScene(Level2)

void
Level2_Init(void) {
	Amphora_SetMap("Grassland", 2);
}

void
Level2_Update(Uint32 frame, const InputState *input) {
	(void)frame;

	if (input->left) {
		Amphora_MoveCamera(-1 ,0);
	}
	if (input->right) {
		Amphora_MoveCamera(1, 0);
	}
	if (input->up) {
		Amphora_MoveCamera(0, -1);
	}
	if (input->down) {
		Amphora_MoveCamera(0, 1);
	}
	if (input->quit) {
		Amphora_QuitGame();
	}
	if (input->scene1) {
		Amphora_LoadScene("Level1");
	}
}

void
Level2_Destroy(void) {

}
