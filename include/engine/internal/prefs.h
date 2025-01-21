#ifndef AMPHORA_INTERNAL_CONFIG_H
#define AMPHORA_INTERNAL_CONFIG_H

#include "SDL.h"

int Amphora_InitConfig(void);
int Amphora_SaveWinX(int win_x);
int Amphora_SaveWinY(int win_y);
int Amphora_SaveWinFlags(Uint64 win_flags);
int Amphora_SaveFPS(Uint32 framerate);
Sint64 Amphora_LoadWinX(void);
Sint64 Amphora_LoadWinY(void);
Uint64 Amphora_LoadWinFlags(void);
Sint64 Amphora_LoadFPS(void);

#endif /* AMPHORA_INTERNAL_CONFIG_H */
