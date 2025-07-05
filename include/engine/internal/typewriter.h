#ifndef AMPHORA_TYPEWRITER_INTERNAL_H
#define AMPHORA_TYPEWRITER_INTERNAL_H

#include "engine/typewriter.h"

struct amphora_typewriter_t {
	AmphoraString *string;
	Uint32 ticker, ms, last_update;
	bool used : 1;
};

#endif /* AMPHORA_TYPEWRITER_INTERNAL_H */
