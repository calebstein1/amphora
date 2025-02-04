#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/error.h"
#include "engine/internal/hash_table.h"
#include "engine/internal/lib.h"
#include "engine/internal/img.h"
#include "engine/internal/render.h"

#include "config.h"

/* Prototypes for private functions */
static int get_img_by_name(const char *name);
static int find_frameset(const AmphoraImage *spr, const char *name);

/* File-scoped variables */
static HT_HashTable images[IMAGES_COUNT * 4 / 2];
static HT_HashTable open_images[IMAGES_COUNT * 4 / 2];
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
		.x = spr->rectangle.x + ((float)spr->framesets[spr->current_frameset].w / 2) - spr->framesets[spr->current_frameset].position_offset.x,
		.y = spr->rectangle.y + ((float)spr->framesets[spr->current_frameset].h / 2) - spr->framesets[spr->current_frameset].position_offset.y
	};
}

bool
Amphora_IsSpriteFlipped(const AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, false)

	return spr->flip;
}

AmphoraImage *
Amphora_CreateSprite(AmphoraImage **spr, const char *image_name, const float x, const float y, const float scale, const bool flip, const bool stationary, const Sint32 order) {
	AmphoraImage *new_sprite = NULL;
	struct render_list_node_t *render_list_node = NULL;
	int idx;

	if (*spr) return *spr;

	if ((idx = get_img_by_name(image_name)) == -1) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Unable to locate image %s\n", image_name);
		return NULL;
	}

	if (!Amphora_HTCheckKeyExists(image_name, open_images)) {
		Amphora_HTSetValue(image_name, SDL_Texture *,
				   IMG_LoadTexture_RW(Amphora_GetRenderer(),
						      Amphora_HTGetValue(image_name, SDL_RWops *, images), 0),
				   open_images);
	}

	if ((new_sprite = SDL_calloc(1, sizeof(AmphoraImage))) == NULL) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to initialize sprite\n");
		*spr = NULL;

		return NULL;
	}
	render_list_node = Amphora_AddRenderListNode(order);

	*spr = new_sprite;

	(*spr)->type = AMPH_OBJ_SPR;
	(*spr)->image = idx;
	(*spr)->rectangle.x = x;
	(*spr)->rectangle.y = y;
	(*spr)->scale = scale;
	(*spr)->render_list_node = render_list_node;
	(*spr)->flip = flip;
	render_list_node->type = AMPH_OBJ_SPR;
	render_list_node->data = *spr;
	render_list_node->stationary = stationary;

	return *spr;
}

int
Amphora_AddFrameset(AmphoraImage *spr, const char *name, const char *override_img, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, float off_x, float off_y, Uint16 num_frames, Uint16 delay) {
	int override = -1;

	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED)
	/* TODO: Cascade error cases and free */
	if (spr->framesets) {
		if (!((spr->framesets = SDL_realloc(spr->framesets, (spr->num_framesets + 1) * sizeof(struct frameset_t))))) {
			Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate framesets\n");
			return AMPHORA_STATUS_ALLOC_FAIL;
		}
		if (!((spr->frameset_labels = SDL_realloc(spr->frameset_labels, (spr->num_framesets + 1) * sizeof(char *))))) {
			Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to reallocate frameset labels\n");
			return AMPHORA_STATUS_ALLOC_FAIL;
		}
	} else {
		if (!((spr->framesets = SDL_malloc(sizeof(struct frameset_t))))) {
			Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate framesets\n");
			return AMPHORA_STATUS_ALLOC_FAIL;
		}
		if (!((spr->frameset_labels = SDL_malloc(sizeof(char *))))) {
			Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate frameset labels\n");
			return AMPHORA_STATUS_ALLOC_FAIL;
		}
	}

	if (override_img) {
		override = get_img_by_name(override_img);
		if (override > -1 && !Amphora_HTGetValue(img_names[override], SDL_Texture *, open_images)) {
			Amphora_HTGetValue(img_names[override], SDL_Texture *, images) =
				IMG_LoadTexture_RW(Amphora_GetRenderer(),
						   Amphora_HTGetValue(img_names[override], SDL_RWops *, images),
						   0);
		}
	}

	spr->framesets[spr->num_framesets] = (struct frameset_t){
		.override_img = override,
		.sx = sx,
		.sy = sy,
		.w = w,
		.h = h,
		.current_frame = -1,
		.num_frames = num_frames,
		.delay = delay,
		.position_offset = (Vector2f){off_x, off_y }
	};
	if (!((spr->frameset_labels[spr->num_framesets] = SDL_malloc(strlen(name) + 1)))) {
		Amphora_SetError(AMPHORA_STATUS_ALLOC_FAIL, "Failed to allocate frameset label\n");
		return AMPHORA_STATUS_ALLOC_FAIL;
	}
	SDL_strlcpy(spr->frameset_labels[spr->num_framesets], name, strlen(name) + 1);
	if (++spr->num_framesets == 1) {
		spr->rectangle.w = (float)spr->framesets[0].w * spr->scale;
		spr->rectangle.h = (float)spr->framesets[0].h * spr->scale;
	}

	return AMPHORA_STATUS_OK;
}

int
Amphora_SetFrameset(AmphoraImage *spr, const char *name) {
	int frameset;

	if ((frameset = find_frameset(spr, name)) == -1) {
		Amphora_SetError(AMPHORA_STATUS_FAIL_UNDEFINED, "Failed to locate frameset: %s\n", name);
		return AMPHORA_STATUS_FAIL_UNDEFINED;
	}
	if (frameset == spr->current_frameset) return AMPHORA_STATUS_OK;

	spr->framesets[frameset].playing_oneshot = false;
	spr->current_frameset = frameset;
	spr->rectangle.w = (float)spr->framesets[frameset].w * spr->scale;
	spr->rectangle.h = (float)spr->framesets[frameset].h * spr->scale;

	return AMPHORA_STATUS_OK;
}

int
Amphora_PlayOneshot(AmphoraImage *spr, const char *name, void (*callback)(void)) {
	int frameset;

	if ((frameset = find_frameset(spr, name)) == -1) {
		Amphora_SetError(AMPHORA_STATUS_FAIL_UNDEFINED, "Failed to locate frameset: %s\n", name);
		return AMPHORA_STATUS_FAIL_UNDEFINED;
	}
	if (frameset == spr->current_frameset) return AMPHORA_STATUS_OK;

	spr->framesets[frameset].playing_oneshot = true;
	spr->current_frameset = frameset;
	spr->rectangle.w = (float)spr->framesets[frameset].w * spr->scale;
	spr->rectangle.h = (float)spr->framesets[frameset].h * spr->scale;
	spr->framesets[frameset].current_frame = -1;
	spr->framesets[frameset].last_change = SDL_GetTicks64();
	spr->framesets[frameset].callback = callback;

	return AMPHORA_STATUS_OK;
}

int
Amphora_SetFramesetAnimationTime(AmphoraImage *spr, const char *name, Uint16 delay) {
	int frameset;

	if ((frameset = find_frameset(spr, name)) == -1) {
		Amphora_SetError(AMPHORA_STATUS_FAIL_UNDEFINED, "Failed to locate frameset: %s\n", name);
		return AMPHORA_STATUS_FAIL_UNDEFINED;
	}
	spr->framesets[frameset].delay = delay;

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
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED);

	spr->rectangle.x = x;
	spr->rectangle.y = y;

	return AMPHORA_STATUS_OK;
}

int
Amphora_MoveSprite(AmphoraImage *spr, float delta_x, float delta_y) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED);

	spr->rectangle.x += delta_x;
	spr->rectangle.y += delta_y;

	return AMPHORA_STATUS_OK;
}

int
Amphora_FlipSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED);

	spr->flip = true;

	return AMPHORA_STATUS_OK;
}

int
Amphora_UnflipSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED);

	spr->flip = false;

	return AMPHORA_STATUS_OK;
}

int
Amphora_ShowSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED);

	spr->render_list_node->display = true;

	return AMPHORA_STATUS_OK;
}

int
Amphora_HideSprite(AmphoraImage *spr) {
	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED);

	spr->render_list_node->display = false;

	return AMPHORA_STATUS_OK;
}

int
Amphora_FreeSprite(AmphoraImage **spr) {
	int i;

	Amphora_ValidatePtrNotNull(spr, AMPHORA_STATUS_FAIL_UNDEFINED);
	Amphora_ValidatePtrNotNull(*spr, AMPHORA_STATUS_FAIL_UNDEFINED);

	if (*spr == Amphora_GetCameraTarget()) Amphora_SetCameraTarget(NULL);
	for (i = 0; i < (*spr)->num_framesets; i++) {
		SDL_free((*spr)->frameset_labels[i]);
	}
	if ((*spr)->frameset_labels) SDL_free((*spr)->frameset_labels);
	if ((*spr)->framesets) SDL_free((*spr)->framesets);
	(*spr)->render_list_node->garbage = true;
	SDL_free(*spr);
	*spr = NULL;

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
		Amphora_HTSetValue(img_names[i], SDL_RWops *, SDL_RWFromConstMem(img_resource, SizeofResource(NULL, img_info)), images);
	}
#else
#define LOADIMG(name, path) extern char name##_im[]; extern int name##_im_size;
	IMAGES
#undef LOADIMG
#define LOADIMG(name, path) Amphora_HTSetValue(#name, SDL_RWops *, SDL_RWFromConstMem(name##_im, name##_im_size), images);
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
Amphora_CloseIMG(void) {
	int i;

	for (i = 0; i < IMAGES_COUNT; i++) {
		SDL_RWclose(Amphora_HTGetValue(img_names[i], SDL_RWops *, images));
		if (Amphora_HTCheckKeyExists(img_names[i], open_images))
			SDL_DestroyTexture(Amphora_HTGetValue(img_names[i], SDL_Texture *, open_images));
	}
}

SDL_Texture *
Amphora_GetIMGTextureByName(const char *name) {
	if (!Amphora_HTCheckKeyExists(name, open_images)) {
		Amphora_HTSetValue(name, SDL_Texture *,
				   IMG_LoadTexture_RW(Amphora_GetRenderer(),
						      Amphora_HTGetValue(name, SDL_RWops *, images), 0),
						      open_images);
	}

	return Amphora_HTGetValue(name, SDL_Texture *, open_images);
}

void
Amphora_UpdateAndDrawSprite(const AmphoraImage *spr) {
	struct frameset_t *frameset = &spr->framesets[spr->current_frameset];
	int frameset_idx = spr->current_frameset;
	SDL_Rect src;
	SDL_FRect dst;
	const Vector2f camera = Ampohra_GetCamera();
	Vector2 logical_size = Amphora_GetRenderLogicalSize();
	Uint64 cur_ms = SDL_GetTicks64();

	if (!(spr->render_list_node->display && spr->num_framesets > 0)) return;

	if (cur_ms - frameset->last_change > frameset->delay) {
		if (++frameset->current_frame == frameset->num_frames) {
			if (spr->framesets[frameset_idx].playing_oneshot) {
				frameset->current_frame--;
				if (spr->framesets[frameset_idx].callback) spr->framesets[frameset_idx].callback();
			} else {
				frameset->current_frame = 0;
			}
		}
		frameset->last_change = cur_ms;
	}
	if (frameset->current_frame == -1) frameset->current_frame = 0;
	src = (SDL_Rect){
		.x = frameset->sx + (frameset->w * frameset->current_frame),
		.y = frameset->sy,
		.w = frameset->w,
		.h = frameset->h
	};
	if (spr->render_list_node->stationary) {
		dst = (SDL_FRect){
			.x = spr->rectangle.x > 0 ? spr->rectangle.x : (float) Amphora_GetResolution().x + spr->rectangle.x - (float)frameset->w,
			.y = spr->rectangle.y > 0 ? spr->rectangle.y : (float) Amphora_GetResolution().y + spr->rectangle.y - (float)frameset->h,
			.w = (float)frameset->w * spr->scale,
			.h = (float)frameset->h * spr->scale
		};
	} else {
		dst = (SDL_FRect){
			.x = spr->rectangle.x - frameset->position_offset.x - camera.x,
			.y = spr->rectangle.y - frameset->position_offset.y - camera.y,
			.w = (float)frameset->w * spr->scale,
			.h = (float)frameset->h * spr->scale
		};
	}

	if (spr->render_list_node->stationary) Amphora_SetRenderLogicalSize(Amphora_GetResolution());
	Amphora_RenderTexture(Amphora_HTGetValue(img_names[(int)frameset->override_img > -1 ?
					frameset->override_img : spr->image], SDL_Texture *, open_images),
			      &src, &dst, 0, spr->flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	if (spr->render_list_node->stationary) Amphora_SetRenderLogicalSize(logical_size);
}

/*
 * Private functions
 */

static int
get_img_by_name(const char *name) {
	int i;

	for (i = 0; i < IMAGES_COUNT; i++) {
		if (SDL_strcmp(name, img_names[i]) == 0) return i;
	}
	return -1;
}

static int
find_frameset(const AmphoraImage *spr, const char *name) {
	int i;

	for (i = 0; i < spr->num_framesets; i++) {
		if (SDL_strcmp(spr->frameset_labels[i], name) == 0) break;
	}
	if (i == spr->num_framesets) {
		return -1;
	}

	return i;
}
