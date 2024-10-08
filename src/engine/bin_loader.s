	#include "config.h"

	.section .rodata,"a"

	.global _spritesheet
	.balign 4
_spritesheet:
	.incbin SPRITESHEET_PATH
_spritesheet_end:
	.global _spritesheet_size
	.balign 4
_spritesheet_size:
	.int _spritesheet_end - _spritesheet
