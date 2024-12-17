#ifndef AMPHORA_INTERNAL_CONFIG_H
#define AMPHORA_INTERNAL_CONFIG_H

#include "SDL.h"

int init_config(void);
int save_window_x(int win_x);
int save_window_y(int win_y);
int save_win_flags(Uint64 win_flags);
int save_framerate(Uint32 framerate);
Sint64 load_window_x(void);
Sint64 load_window_y(void);
Uint64 load_win_flags(void);
Sint64 load_framerate(void);

#endif /* AMPHORA_INTERNAL_CONFIG_H */
