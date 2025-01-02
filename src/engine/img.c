#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/img.h"
#include "engine/internal/render.h"
#include "engine/internal/timer.h"

#include "config.h"

/* Prototypes for private functions */
int get_img_by_name(const char *name);
int find_frameset(const AmphoraImage *spr, const char *name);

/* File-scoped variables */
static SDL_RWops *images[IMAGES_COUNT];
static SDL_Texture *open_images[IMAGES_COUNT];
static const char *img_names[] = {
#define LOADIMG(name, path) #name,
	IMAGES
#undef LOADIMG
};

Vector2f
get_sprite_position(const AmphoraImage *spr) {
	return (Vector2f){spr->rectangle.x, spr->rectangle.y };
}

Vector2f
get_sprite_center(const AmphoraImage *spr) {
	return (Vector2f){
		.x = spr->rectangle.x + ((float)spr->framesets[spr->current_frameset].w / 2) - spr->framesets[spr->current_frameset].position_offset.x,
		.y = spr->rectangle.y + ((float)spr->framesets[spr->current_frameset].h / 2) - spr->framesets[spr->current_frameset].position_offset.y
	};
}

bool
is_flipped(const AmphoraImage *spr) {
	return spr->flip;
}

AmphoraImage *
create_sprite(AmphoraImage **spr, const char *image_name, const float x, const float y, const float scale, const bool flip, const bool stationary, const Sint32 order) {
	AmphoraImage *new_sprite = NULL;
	struct render_list_node_t *render_list_node = NULL;
	int idx;

	if (*spr) return *spr;

	if ((idx = get_img_by_name(image_name)) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to locate image %s\n", image_name);
		return NULL;
	}

	if (!open_images[idx]) {
		open_images[idx] = IMG_LoadTexture_RW(get_renderer(), images[idx], 0);
	}

	if ((new_sprite = SDL_calloc(1, sizeof(AmphoraImage))) == NULL) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to initialize sprite\n");
		*spr = NULL;

		return NULL;
	}
	render_list_node = add_render_list_node(order);

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

void
add_frameset(AmphoraImage *spr, const char *name, const Sint32 sx, const Sint32 sy, const Sint32 w, const Sint32 h, const float off_x, const float off_y, const Uint16 num_frames, const Uint16 delay) {
	/* TODO: Cascade error cases and free */
	if (spr->framesets) {
		if (!((spr->framesets = SDL_realloc(spr->framesets, (spr->num_framesets + 1) * sizeof(struct frameset_t))))) {
			SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to reallocate framesets\n");
			return;
		}
		if (!((spr->frameset_labels = SDL_realloc(spr->frameset_labels, (spr->num_framesets + 1) * sizeof(char *))))) {
			SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to reallocate frameset labels\n");
			return;
		}
	} else {
		if (!((spr->framesets = SDL_malloc(sizeof(struct frameset_t))))) {
			SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to allocate framesets\n");
			return;
		}
		if (!((spr->frameset_labels = SDL_malloc(sizeof(char *))))) {
			SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to allocate frameset labels\n");
			return;
		}
	}
	spr->framesets[spr->num_framesets] = (struct frameset_t){
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
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to allocate frameset label\n");
		return;
	}
	SDL_strlcpy(spr->frameset_labels[spr->num_framesets], name, strlen(name) + 1);
	if (++spr->num_framesets == 1) {
		spr->rectangle.w = (float)spr->framesets[0].w * spr->scale;
		spr->rectangle.h = (float)spr->framesets[0].h * spr->scale;
	}
}

void
set_frameset(AmphoraImage *spr, const char *name) {
	int frameset;

	if ((frameset = find_frameset(spr, name)) == -1) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to locate frameset: %s\n", name);
		return;
	}
	spr->framesets[frameset].playing_oneshot = false;
	spr->current_frameset = frameset;
	spr->rectangle.w = (float)spr->framesets[frameset].w * spr->scale;
	spr->rectangle.h = (float)spr->framesets[frameset].h * spr->scale;
}

void
play_oneshot(AmphoraImage *spr, const char *name, void (*callback)(void)) {
	int frameset;

	if ((frameset = find_frameset(spr, name)) == -1) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to locate frameset: %s\n", name);
		return;
	}
	spr->framesets[frameset].playing_oneshot = true;
	spr->current_frameset = frameset;
	spr->rectangle.w = (float)spr->framesets[frameset].w * spr->scale;
	spr->rectangle.h = (float)spr->framesets[frameset].h * spr->scale;
	spr->framesets[frameset].current_frame = -1;
	spr->framesets[frameset].last_change = SDL_GetTicks64();
	spr->framesets[frameset].callback = callback;
}

void
set_frameset_delay(AmphoraImage *spr, const char *name, const Uint16 delay) {
	int frameset;

	if ((frameset = find_frameset(spr, name)) == -1) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to locate frameset: %s\n", name);
		return;
	}
	spr->framesets[frameset].delay = delay;
}

AmphoraImage *
reorder_sprite(AmphoraImage *spr, const Sint32 order) {
	struct render_list_node_t *new_node = add_render_list_node(order);
	struct render_list_node_t *old_node = spr->render_list_node;

	new_node->type = AMPH_OBJ_SPR;
	new_node->data = spr;
	new_node->display = old_node->display;
	new_node->stationary = old_node->stationary;
	old_node->garbage = true;
	spr->render_list_node = new_node;

	return spr;
}

void
set_sprite_location(AmphoraImage *spr, float x, float y) {
	spr->rectangle.x = x;
	spr->rectangle.y = y;
}

void
move_sprite(AmphoraImage *spr, const float delta_x, const float delta_y) {
	spr->rectangle.x += delta_x;
	spr->rectangle.y += delta_y;
}

void
flip_sprite(AmphoraImage *spr) {
	spr->flip = true;
}

void
unflip_sprite(AmphoraImage *spr) {
	spr->flip = false;
}

void
show_sprite(AmphoraImage *spr) {
	spr->render_list_node->display = true;
}

void
hide_sprite(AmphoraImage *spr) {
	spr->render_list_node->display = false;
}

void
free_sprite(AmphoraImage **spr) {
	int i;

	if (!*spr) return;

	for (i = 0; i < (*spr)->num_framesets; i++) {
		SDL_free((*spr)->frameset_labels[i]);
	}
	if ((*spr)->frameset_labels) SDL_free((*spr)->frameset_labels);
	if ((*spr)->framesets) SDL_free((*spr)->framesets);
	(*spr)->render_list_node->garbage = true;
	SDL_free(*spr);
	*spr = NULL;
}

/*
 * Internal functions
 */

int
init_img(void) {
	int i;
#ifdef WIN32
	HRSRC img_info;
	HGLOBAL img_resource;
	SDL_RWops *img_rw;

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
		img_rw = SDL_RWFromConstMem(img_resource, SizeofResource(NULL, img_info));
		images[i] = img_rw;
	}
#else
#define LOADIMG(name, path) extern char name##_im[]; extern int name##_im_size;
	IMAGES
#undef LOADIMG
	SDL_RWops **img_ptr = images;
#define LOADIMG(name, path) *img_ptr = SDL_RWFromConstMem(name##_im, name##_im_size); img_ptr++;
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
cleanup_img(void) {
	int i;

	for (i = 0; i < IMAGES_COUNT; i++) {
		SDL_RWclose(images[i]);
		if (open_images[i]) SDL_DestroyTexture(open_images[i]);
	}
}

SDL_Texture *
get_img_texture_by_name(const char *name) {
	int idx;

	if ((idx = get_img_by_name(name)) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to locate image %s\n", name);
		return NULL;
	}

	if (!open_images[idx]) {
		open_images[idx] = IMG_LoadTexture_RW(get_renderer(), images[idx], 0);
	}

	return open_images[idx];
}

void
update_and_draw_sprite(const AmphoraImage *spr) {
	struct frameset_t *frameset = &spr->framesets[spr->current_frameset];
	int frameset_idx = spr->current_frameset;
	SDL_Rect src;
	SDL_FRect dst;
	const Vector2f camera = get_camera();
	Vector2 logical_size = get_render_logical_size();
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
			.x = spr->rectangle.x > 0 ? spr->rectangle.x : (float)get_resolution().x + spr->rectangle.x - (float)frameset->w,
			.y = spr->rectangle.y > 0 ? spr->rectangle.y : (float)get_resolution().y + spr->rectangle.y - (float)frameset->h,
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

	if (spr->render_list_node->stationary) set_render_logical_size(get_resolution());
	render_texture(open_images[spr->image], &src, &dst, 0, spr->flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	if (spr->render_list_node->stationary) set_render_logical_size(logical_size);
}

/*
 * Private functions
 */

int
get_img_by_name(const char *name) {
	int i;

	for (i = 0; i < IMAGES_COUNT; i++) {
		if (SDL_strcmp(name, img_names[i]) == 0) return i;
	}
	return -1;
}

int
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
