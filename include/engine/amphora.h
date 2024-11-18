#ifndef AMPHORA_H
#define AMPHORA_H

#include "config.h"

#ifdef EXPOSE_INTERNAL_DEFINITIONS_AND_FUNCTIONS
#include "engine/internal/events.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"
#include "engine/internal/save_data.h"
#include "engine/internal/tilemap.h"
#include "engine/internal/timer.h"
#include "engine/internal/ttf.h"
#include "engine/game_loop.h"
#include "engine/util.h"
#else
#include "engine/events.h"
#include "engine/game_loop.h"
#include "engine/img.h"
#include "engine/input.h"
#include "engine/render.h"
#include "engine/save_data.h"
#include "engine/tilemap.h"
#include "engine/ttf.h"
#include "engine/util.h"
#endif

#endif /* AMPHORA_H */
