#define ASM
#include "config.h"

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

#undef ASM
