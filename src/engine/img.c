#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/error.h"
#include "engine/internal/lib.h"
#include "engine/internal/img.h"
#include "engine/internal/render.h"

#include "config.h"

/* File-scoped variables */
static HT_HashTable images;
static HT_HashTable open_images;
static const char *img_names[] = {
#define LOADIMG(name, path) #name,
	IMAGES
#undef LOADIMG
};

Vector2f
Amphora_GetSpritePosition(const AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, ((Vector2f){0, 0 }))

	return (Vector2f){spr->rectangle.x, spr->rectangle.y };
}

Vector2f
Amphora_GetSpriteCenter(const AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, ((Vector2f){0, 0 }))

	return (Vector2f){
		.x = spr->rectangle.x + ((float)spr->frameset_list[spr->current_frameset].rectangle.w / 2) - spr->frameset_list[spr->current_frameset].position_offset.x,
		.y = spr->rectangle.y + ((float)spr->frameset_list[spr->current_frameset].rectangle.h / 2) - spr->frameset_list[spr->current_frameset].position_offset.y
	};
}

bool
Amphora_IsSpriteFlipped(const AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, false)

	return spr->flip;
}

AmphoraImage *
Amphora_CreateSprite(const char *image_name, const float x, const float y, const float scale, const bool flip, const bool stationary, const Sint32 order) {
	AmphoraImage *spr = NULL;
	struct render_list_node_t *render_list_node = NULL;
	SDL_RWops *img_rw = NULL;

	if (!HT_GetValue(image_name, open_images)) {
#ifdef DEBUG
		SDL_Log("Loading image: %s\n", image_name);
#endif
		img_rw = SDL_RWFromConstMem(HT_GetRef(image_name, char, images), HT_GetStatus(image_name, images));
		HT_StoreRef(image_name, IMG_LoadTexture_RW(Amphora_GetRenderer(), img_rw, 1), open_images);
	}

	if ((spr = SDL_calloc(1, sizeof(AmphoraImage))) == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to initialize sprite\n");

		return NULL;
	}
	render_list_node = Amphora_AddRenderListNode(order);

	spr->type = AMPH_OBJ_SPR;
	spr->image = HT_GetRef(image_name, SDL_Texture, open_images);
	spr->rectangle.x = x;
	spr->rectangle.y = y;
	spr->scale = scale;
	spr->framesets = HT_NewTable();
	spr->render_list_node = render_list_node;
	spr->flip = flip;
	render_list_node->type = AMPH_OBJ_SPR;
	render_list_node->data = spr;
	render_list_node->stationary = stationary;

	return spr;
}

int
Amphora_AddFrameset(AmphoraImage *spr, const char *name, const char *override_img, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, float off_x, float off_y, Uint16 num_frames, Uint16 delay) {
	SDL_Texture *override = NULL;
	SDL_RWops *img_rw = NULL;

	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)
    if (!((spr->frameset_list = SDL_realloc(spr->frameset_list, (spr->num_framesets + 1) * sizeof(struct frameset_t))))) {
        Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate framesets\n");
        return AMPHORA_STATUS_ALLOC_FAIL;
    }

	if (override_img) {
        if (!HT_GetValue(override_img, open_images)) {
#ifdef DEBUG
            SDL_Log("Loading image: %s\n", override_img);
#endif
            img_rw = SDL_RWFromConstMem(HT_GetRef(override_img, char, images), HT_GetStatus(override_img, images));
            HT_StoreRef(override_img, IMG_LoadTexture_RW(Amphora_GetRenderer(), img_rw, 1), open_images);
        }
		override = HT_GetRef(override_img, SDL_Texture, open_images);
	}

	spr->frameset_list[spr->num_framesets] = (struct frameset_t){
		.override_img = override,
		.rectangle.x = sx,
		.rectangle.y = sy,
		.rectangle.w = w,
		.rectangle.h = h,
		.current_frame = -1,
		.num_frames = num_frames,
		.delay = delay,
		.position_offset = (Vector2f){off_x, off_y }
	};
	HT_SetValue(name, spr->num_framesets, spr->framesets);
	if (++spr->num_framesets == 1) {
		spr->current_frameset = 0;
		spr->rectangle.w = (float)spr->frameset_list[0].rectangle.w * spr->scale;
		spr->rectangle.h = (float)spr->frameset_list[0].rectangle.h * spr->scale;
	}

	return AMPHORA_STATUS_OK;
}

void
Amphora_SetFrameset(AmphoraImage *spr, const char *name) {
	int idx = (int)HT_GetValue(name, spr->framesets);
	struct frameset_t *frameset = &spr->frameset_list[idx];

	if (idx == spr->current_frameset) return;

	frameset->playing_oneshot = false;
	spr->current_frameset = idx;
	spr->rectangle.w = (float)frameset->rectangle.w * spr->scale;
	spr->rectangle.h = (float)frameset->rectangle.h * spr->scale;
}

void
Amphora_PlayOneshot(AmphoraImage *spr, const char *name, void (*callback)(void)) {
	int idx = (int)HT_GetValue(name, spr->framesets);
	struct frameset_t *frameset = &spr->frameset_list[idx];

	if (idx == spr->current_frameset) return;

	frameset->playing_oneshot = true;
	spr->current_frameset = idx;
	spr->rectangle.w = (float)frameset->rectangle.w * spr->scale;
	spr->rectangle.h = (float)frameset->rectangle.h * spr->scale;
	frameset->current_frame = -1;
	frameset->last_change = SDL_GetTicks();
	frameset->callback = callback;
}

int
Amphora_SetFramesetAnimationTime(AmphoraImage *spr, const char *name, Uint16 delay) {
	struct frameset_t *frameset = &spr->frameset_list[HT_GetValue(name, spr->framesets)];

	if (!frameset) return AMPHORA_STATUS_FAIL_UNDEFINED;
	frameset->delay = delay;

	return AMPHORA_STATUS_OK;
}

AmphoraImage *
Amphora_ReorderSprite(AmphoraImage *spr, Sint32 order) {
	struct render_list_node_t *new_node = Amphora_AddRenderListNode(order);
	struct render_list_node_t *old_node = spr->render_list_node;

	Amphora_ValidatePtrNotNull(new_node, NULL)

	new_node->type = AMPH_OBJ_SPR;
	new_node->data = spr;
	new_node->display = old_node->display;
	new_node->stationary = old_node->stationary;
	old_node->garbage = true;
	spr->render_list_node = new_node;

	return spr;
}

int
Amphora_SetSpriteLocation(AmphoraImage *spr, float x, float y) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)

	spr->rectangle.x = x;
	spr->rectangle.y = y;

	return AMPHORA_STATUS_OK;
}

int
Amphora_MoveSprite(AmphoraImage *spr, float delta_x, float delta_y) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)

	spr->rectangle.x += delta_x;
	spr->rectangle.y += delta_y;

	return AMPHORA_STATUS_OK;
}

int
Amphora_FlipSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)

	spr->flip = true;

	return AMPHORA_STATUS_OK;
}

int
Amphora_UnflipSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)

	spr->flip = false;

	return AMPHORA_STATUS_OK;
}

int
Amphora_ShowSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)

	spr->render_list_node->display = true;

	return AMPHORA_STATUS_OK;
}

int
Amphora_HideSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)

	spr->render_list_node->display = false;

	return AMPHORA_STATUS_OK;
}

int
Amphora_FreeSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)

	if (spr == Amphora_GetCameraTarget()) Amphora_SetCameraTarget(NULL);
	if (spr->frameset_list) SDL_free(spr->frameset_list);
	HT_FreeTable(spr->framesets);
	spr->render_list_node->garbage = true;
	SDL_free(spr);
	spr = NULL;

	return AMPHORA_STATUS_OK;
}

/*
 * Internal functions
 */

int
Amphora_InitIMG(void) {
	int i;
#ifdef WIN32
	HRSRC img_info;
	HGLOBAL img_resource;

	images = HT_NewTable();
	open_images = HT_NewTable();

	for (i = 0; i < IMAGES_COUNT; i++) {
		if (!((img_info = FindResourceA(NULL, img_names[i], "PNG_IMG")))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to locate image resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to locate image resource... Amphora will crash now", 0);
			return -1;
		}
		if (!((img_resource = LoadResource(NULL, img_info)))) {
			SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Failed to load image resource... Amphora will crash now\n");
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Resource load error", "Failed to load image resource... Amphora will crash now", 0);
			return -1;
		}
		HT_StoreRef(img_names[i], img_resource, images);
		HT_SetStatus(img_names[i], SizeofResource(NULL, img_info), images);
	}
#else
#define LOADIMG(name, path) extern char name##_im[]; extern int name##_im_size;
	IMAGES
#undef LOADIMG
	images = HT_NewTable();
	open_images = HT_NewTable();
#define LOADIMG(name, path) HT_StoreRef(#name, name##_im, images); \
							HT_SetStatus(#name, name##_im_size, images);
	IMAGES
#undef LOADIMG
#endif
#ifdef DEBUG
	for (i = 0; i < IMAGES_COUNT; i++) {
		SDL_Log("Found image %s\n", img_names[i]);
	}
#endif

	return 0;
}

void
Amphora_FreeAllIMG(void) {
	int i;

	for (i = 0; i < IMAGES_COUNT; i++) {
		if (HT_GetValue(img_names[i], open_images)) {
#ifdef DEBUG
			SDL_Log("Unloading image: %s\n", img_names[i]);
#endif
			SDL_DestroyTexture(HT_GetRef(img_names[i], SDL_Texture, open_images));
			HT_SetValue(img_names[i], 0, open_images);
			HT_DeleteKey(img_names[i], open_images);
		}
	}
}

void
Amphora_CloseIMG(void) {
	HT_FreeTable(images);
	HT_FreeTable(open_images);
}

SDL_Texture *
Amphora_GetIMGTextureByName(const char *name) {
	SDL_RWops *img_rw = NULL;

	if (!HT_GetValue(name, open_images)) {
#ifdef DEBUG
		SDL_Log("Loading image: %s\n", name);
#endif
		img_rw = SDL_RWFromConstMem(HT_GetRef(name, char, images), HT_GetStatus(name, images));
		HT_StoreRef(name, IMG_LoadTexture_RW(Amphora_GetRenderer(), img_rw, 1), open_images);
	}

	return HT_GetRef(name, SDL_Texture, open_images);
}

void
Amphora_UpdateAndDrawSprite(const AmphoraImage *spr) {
	struct frameset_t *frameset = &spr->frameset_list[spr->current_frameset];
	SDL_Rect src;
	SDL_FRect dst;
	const Vector2f camera = Ampohra_GetCamera();
	Vector2 logical_size = Amphora_GetRenderLogicalSize();
	Uint32 cur_ms = SDL_GetTicks();

	if (!(spr->render_list_node->display && spr->num_framesets > 0)) return;

	if (cur_ms - frameset->last_change > frameset->delay) {
		if (++frameset->current_frame == frameset->num_frames) {
			if (frameset->playing_oneshot) {
				frameset->current_frame--;
				if (frameset->callback) frameset->callback();
			} else {
				frameset->current_frame = 0;
			}
		}
		frameset->last_change = cur_ms;
	}
	if (frameset->current_frame == -1) frameset->current_frame = 0;
	src = frameset->rectangle;
	src.x += (src.w * frameset->current_frame);
	if (spr->render_list_node->stationary) {
		dst = (SDL_FRect){
			.x = spr->rectangle.x > 0 ? spr->rectangle.x : (float) Amphora_GetResolution().x + spr->rectangle.x - (float)frameset->rectangle.w,
			.y = spr->rectangle.y > 0 ? spr->rectangle.y : (float) Amphora_GetResolution().y + spr->rectangle.y - (float)frameset->rectangle.h,
			.w = (float)frameset->rectangle.w * spr->scale,
			.h = (float)frameset->rectangle.h * spr->scale
		};
	} else {
		dst = (SDL_FRect){
			.x = spr->rectangle.x - frameset->position_offset.x - camera.x,
			.y = spr->rectangle.y - frameset->position_offset.y - camera.y,
			.w = (float)frameset->rectangle.w * spr->scale,
			.h = (float)frameset->rectangle.h * spr->scale
		};
	}

	if (spr->render_list_node->stationary) Amphora_SetRenderLogicalSize(Amphora_GetResolution());
	Amphora_RenderTexture(frameset->override_img ? frameset->override_img : spr->image,
			      &src, &dst, 0, spr->flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	if (spr->render_list_node->stationary) Amphora_SetRenderLogicalSize(logical_size);
}
