#ifdef WIN32
#include <windows.h>
#endif

#include "engine/internal/img.h"
#include "engine/internal/render.h"
#include "engine/internal/timer.h"

#include "config.h"

/* Prototypes for private functions */
int find_frameset(const AmphoraImage *spr, const char *name);
void update_and_draw_sprite(const AmphoraImage *spr);

/* File-scoped variables */
static SDL_RWops *images[IMAGES_COUNT];
static SDL_Texture *open_images[IMAGES_COUNT];
static AmphoraImage *sprite_slot;
static AmphoraImage *sprite_slots_head;
static const char *img_names[] = {
#define LOADIMG(name, path) #name,
	IMAGES
#undef LOADIMG
};
Uint32 sprite_slots_count = 1;

Vector2
get_sprite_center(const AmphoraImage *spr) {
	return (Vector2){
		.x = spr->dx + (spr->framesets[spr->current_frameset].w / 2) - spr->framesets[spr->current_frameset].position_offset.x,
		.y = spr->dy + (spr->framesets[spr->current_frameset].h / 2) - spr->framesets[spr->current_frameset].position_offset.y
	};
}

AmphoraImage *
init_sprite_slot(AmphoraImage **spr, const char *name, const Sint32 x, const Sint32 y, const Uint8 scale, const bool flip, const bool stationary, const Sint32 order) {
	AmphoraImage *sprite_slot_temp = NULL;
	int idx;

	if (*spr) return *spr;

	if ((idx = get_img_by_name(name)) == -1) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to locate image %s\n", name);
		return NULL;
	}

	if (!open_images[idx]) {
		open_images[idx] = IMG_LoadTexture_RW(get_renderer(), images[idx], 0);
	}

	if ((sprite_slot_temp = SDL_calloc(1, sizeof(AmphoraImage))) == NULL) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to initialize sprite\n");
		*spr = NULL;

		return NULL;
	}
	while (1) {
		if (sprite_slot->next == NULL) {
			sprite_slot_temp->next = NULL;
			sprite_slot->next = sprite_slot_temp;
			break;
		}
		if (sprite_slot->next->order >= order) {
			sprite_slot_temp->next = sprite_slot->next;
			sprite_slot->next = sprite_slot_temp;
			break;
		}
		sprite_slot = sprite_slot->next;
	}
	*spr = sprite_slot_temp;
	sprite_slots_count++;
	sprite_slot = sprite_slots_head;

	(*spr)->image = idx;
	(*spr)->dx = x;
	(*spr)->dy = y;
	(*spr)->scale = scale;
	(*spr)->flip = flip;
	(*spr)->stationary = stationary;
	(*spr)->playing_oneshot = false;
	(*spr)->display = true;
	(*spr)->garbage = false;
	(*spr)->order = order;
	(*spr)->callback = NULL;

	return *spr;
}

void
add_frameset(AmphoraImage *spr, const char *name, const Sint32 sx, const Sint32 sy, const Sint32 w, const Sint32 h, const Sint32 off_x, const Sint32 off_y, const Uint16 num_frames, const Uint16 delay) {
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
		.num_frames = num_frames,
		.delay = delay,
		.position_offset = (Vector2){ off_x, off_y }
	};
	if (!((spr->frameset_labels[spr->num_framesets] = SDL_malloc(strlen(name) + 1)))) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to allocate frameset label\n");
		return;
	}
	SDL_strlcpy(spr->frameset_labels[spr->num_framesets], name, strlen(name) + 1);
	spr->num_framesets++;
}

void
set_frameset(AmphoraImage *spr, const char *name) {
	int frameset;

	spr->playing_oneshot = false;
	if ((frameset = find_frameset(spr, name)) == -1) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to locate frameset: %s\n", name);
		return;
	}
	spr->current_frameset = frameset;
}

void
play_oneshot(AmphoraImage *spr, const char *name, const CallbackFn callback) {
	int frameset;

	spr->playing_oneshot = true;
	if ((frameset = find_frameset(spr, name)) == -1) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to locate frameset: %s\n", name);
		return;
	}
	spr->current_frameset = frameset;
	spr->framesets[frameset].current_frame = 0;
	spr->callback = callback;
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
reorder_sprite(AmphoraImage **spr, const Sint32 order) {
	AmphoraImage *sprite_slot_temp = NULL;

	if ((sprite_slot_temp = SDL_malloc(sizeof(AmphoraImage))) == NULL) {
		SDL_LogError(SDL_LOG_PRIORITY_WARN, "Failed to reorder sprite\n");

		return NULL;
	}
	SDL_memcpy(sprite_slot_temp, *spr, sizeof(AmphoraImage));
	sprite_slot_temp->order = order;
	while (1) {
		if (sprite_slot->next == NULL) {
			sprite_slot_temp->next = NULL;
			sprite_slot->next = sprite_slot_temp;
			break;
		}
		if (sprite_slot->next->order >= order) {
			sprite_slot_temp->next = sprite_slot->next;
			sprite_slot->next = sprite_slot_temp;
			break;
		}
		sprite_slot = sprite_slot->next;
	}
	(*spr)->garbage = true;
	*spr = sprite_slot_temp;
	sprite_slots_count++;
	sprite_slot = sprite_slots_head;

	return *spr;
}

void
move_sprite(AmphoraImage *spr, const Sint32 delta_x, const Sint32 delta_y) {
	spr->dx += delta_x;
	spr->dy += delta_y;
}

void
show_sprite(AmphoraImage *spr) {
	spr->display = true;
}

void
hide_sprite(AmphoraImage *spr) {
	spr->display = false;
}

void *
release_sprite_slot(AmphoraImage **spr) {
	int i;

	if (spr) {
		for (i = 0; i < (*spr)->num_framesets; i++) {
			SDL_free((*spr)->frameset_labels[i]);
		}
		if ((*spr)->frameset_labels) SDL_free((*spr)->frameset_labels);
		if ((*spr)->framesets) SDL_free((*spr)->framesets);
		(*spr)->garbage = true;
		*spr = NULL;
	}

	return NULL;
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
	if ((sprite_slot = SDL_malloc(sizeof(AmphoraImage))) == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize sprite slots\n");

		return -1;
	}
	sprite_slot->order = SDL_MIN_SINT32;
	sprite_slot->display = false;
	sprite_slot->garbage = false;
	sprite_slot->next = NULL;
	sprite_slots_head = sprite_slot;

	return 0;
}

void
cleanup_sprites(void) {
	AmphoraImage **allocated_addrs = SDL_malloc(sprite_slots_count * sizeof(AmphoraImage *));
	AmphoraImage *garbage;
	Uint32 i = 0;

	while (sprite_slot) {
		allocated_addrs[i++] = sprite_slot;
		sprite_slot = sprite_slot->next;
	}
	for (i = 0; i < sprite_slots_count; i++) {
		garbage = allocated_addrs[i];
		if (i > 0) release_sprite_slot(&allocated_addrs[i]);
		SDL_free(garbage);
	}
	for (i = 0; i < IMAGES_COUNT; i++) {
		SDL_RWclose(images[i]);
		if (open_images[i]) SDL_DestroyTexture(open_images[i]);
	}
	SDL_free(allocated_addrs);
}

void
draw_all_sprites_and_gc(void) {
	AmphoraImage *garbage;

	if (!sprite_slot->next) return;

	while(1) {
		if (!sprite_slot) break;

		if (sprite_slot->next && sprite_slot->next->garbage) {
			garbage = sprite_slot->next;
			sprite_slot->next = sprite_slot->next->next;
			SDL_free(garbage);
			garbage = NULL;
			sprite_slots_count--;
		}
		if (sprite_slot->display && sprite_slot->num_framesets > 0) update_and_draw_sprite(sprite_slot);

		sprite_slot = sprite_slot->next;
	}

	sprite_slot = sprite_slots_head;
}

int
get_img_by_name(const char *name) {
	int i;

	for (i = 0; i < IMAGES_COUNT; i++) {
		if (SDL_strcmp(name, img_names[i]) == 0) return i;
	}
	return -1;
}

/*
 * Private functions
 */

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

void
update_and_draw_sprite(const AmphoraImage *spr) {
	struct frameset_t *frameset = &spr->framesets[spr->current_frameset];
	SDL_Rect src, dst;
	const Vector2 camera = get_camera();
	Vector2 logical_size = get_render_logical_size();

	if (frame_count - frameset->last_change > frameset->delay) {
		if (++frameset->current_frame == frameset->num_frames) {
			if (spr->playing_oneshot) {
				frameset->current_frame--;
				if (spr->callback) spr->callback();
			} else {
				frameset->current_frame = 0;
			}
		}
		frameset->last_change = frame_count;
	}
	src = (SDL_Rect){
		.x = frameset->sx + (frameset->w * frameset->current_frame),
		.y = frameset->sy,
		.w = frameset->w,
		.h = frameset->h
	};
	if (spr->stationary) {
		dst = (SDL_Rect){
			.x = spr->dx > 0 ? spr->dx : get_resolution().x + spr->dx - frameset->w,
			.y = spr->dy > 0 ? spr->dy : get_resolution().y + spr->dy - frameset->h,
			.w = frameset->w * spr->scale,
			.h = frameset->h * spr->scale
		};
	} else {
		dst = (SDL_Rect){
			.x = spr->dx - frameset->position_offset.x - camera.x,
			.y = spr->dy - frameset->position_offset.y - camera.y,
			.w = frameset->w * spr->scale,
			.h = frameset->h * spr->scale
		};
	}

	if (spr->stationary) set_render_logical_size(get_resolution());
	render_texture(open_images[spr->image], &src, &dst);
	if (spr->stationary) set_render_logical_size(logical_size);
}
