#ifndef AMPHORA_SAVE_DATA_H
#define AMPHORA_SAVE_DATA_H

/* The fields to be saved to the game's save file */
struct save_data_t {
    char player_name[256];
    unsigned int high_score;
};

#endif /* AMPHORA_SAVE_DATA_H */
