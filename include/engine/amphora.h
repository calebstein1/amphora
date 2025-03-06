#ifndef AMPHORA_H
#define AMPHORA_H

#include "config.h"

#ifdef EXPOSE_INTERNAL_DEFINITIONS_AND_FUNCTIONS
#include "engine/internal/collision.h"
#include "engine/internal/db.h"
#include "engine/internal/error.h"
#include "engine/internal/events.h"
#include "engine/internal/img.h"
#include "engine/internal/input.h"
#include "engine/internal/render.h"
#include "engine/internal/mixer.h"
#include "engine/internal/random.h"
#include "engine/internal/save_data.h"
#include "engine/internal/scenes.h"
#include "engine/internal/session_data.h"
#include "engine/internal/tilemap.h"
#include "engine/internal/timer.h"
#include "engine/internal/ttf.h"
#include "engine/util.h"
#else
#include "engine/collision.h"
#include "engine/db.h"
#include "engine/error.h"
#include "engine/events.h"
#include "engine/img.h"
#include "engine/input.h"
#include "engine/mixer.h"
#include "engine/random.h"
#include "engine/render.h"
#include "engine/save_data.h"
#include "engine/scenes.h"
#include "engine/session_data.h"
#include "engine/tilemap.h"
#include "engine/ttf.h"
#include "engine/util.h"
#endif

#endif /* AMPHORA_H */
