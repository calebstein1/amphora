#define BIN_LOADER
#include "config.h"
#include "resources.h"

define(`_incbin',dnl
`
	.global $1
	.balign 4
$1:
	.incbin $2
$1_end:
	.global $1_size
	.balign 4
$1_size:
	.int $1_end - $1
')

	.section .rodata,"a"

#if defined(__APPLE__)
#define LOADIMG(name, path) _incbin(_##name##_im, path)
	IMAGES
#undef LOADIMG
#ifndef DISABLE_FONTS
#define LOADFONT(name, path) _incbin(_##name##_ft, path)
	FONTS
#undef LOADFONT
#endif
#ifndef DISABLE_TILEMAP
#define LOADMAP(name, path) _incbin(_##name##_tm, path)
	MAPS
#undef LOADMAP
#endif
#ifndef DISABLE_MIXER
#define LOADSFX(name, path) _incbin(_##name##_sf, path)
	SFX
#undef LOADSFX
#endif
#else
#define LOADIMG(name, path) _incbin(name##_im, path)
	IMAGES
#undef LOADIMG
#ifndef DISABLE_FONTS
#define LOADFONT(name, path) _incbin(name##_ft, path)
	FONTS
#undef LOADFONT
#endif
#ifndef DISABLE_TILEMAP
#define LOADMAP(name, path) _incbin(name##_tm, path)
	MAPS
#undef LOADMAP
#endif
#ifndef DISABLE_MIXER
#define LOADSFX(name, path) _incbin(name##_sf, path)
	SFX
#undef LOADSFX
#endif
#endif

#undef BIN_LOADER
