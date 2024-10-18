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

#if defined(__APPLE__)
#define SS_SYM _spritesheet
#define SS_END_SYM _spritesheet_end
#define SS_SIZE_SYM _spritesheet_size
#else
#define SS_SYM spritesheet
#define SS_END_SYM spritesheet_end
#define SS_SIZE_SYM spritesheet_size
#endif

	.section .rodata,"a"

	.global SS_SYM
	.balign 4
SS_SYM:
	.incbin SPRITESHEET_PATH
SS_END_SYM:
	.global SS_SIZE_SYM
	.balign 4
SS_SIZE_SYM:
	.int SS_END_SYM - SS_SYM

#if defined(__APPLE__)
#define LOADFONT(name, path) _incbin(_##name, path)
	FONTS
#undef LOADFONT
#else
#define LOADFONT(name, path) _incbin(name, path)
	FONTS
#undef LOADFONT
#endif

#undef BIN_LOADER
