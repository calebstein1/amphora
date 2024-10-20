#define BIN_LOADER
#include "config.h"

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
_incbin(_spritesheet, SPRITESHEET_PATH)
#define LOADIMG(name, path) _incbin(_##name, path)
	IMAGES
#undef LOADIMG
#ifdef ENABLE_FONTS
#define LOADFONT(name, path) _incbin(_##name, path)
	FONTS
#undef LOADFONT
#endif
#else
_incbin(spritesheet, SPRITESHEET_PATH)
#define LOADIMG(name, path) _incbin(name, path)
	IMAGES
#undef LOADIMG
#ifdef ENABLE_FONTS
#define LOADFONT(name, path) _incbin(name, path)
	FONTS
#undef LOADFONT
#endif
#endif

#undef BIN_LOADER
