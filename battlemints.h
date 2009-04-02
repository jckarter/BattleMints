#ifndef __BATTLEMINTS_H__
#define __BATTLEMINTS_H__

#ifdef __cplusplus
extern "C" {
#endif

void battlemints_start(void);

void battlemints_tick(void);
void battlemints_draw(void);

void battlemints_input(float x, float y, int tap_count);

void battlemints_finish(void);

void battlemints_go_to_hub(void);
void battlemints_go_to_start(void);

enum battlemints_pause_flag_values {
    BATTLEMINTS_START_MAP = 1,
    BATTLEMINTS_HUB_MAP = 2,
    BATTLEMINTS_GAME_ACTIVE = 4
};

int battlemints_pause_flags(void);

#ifdef __cplusplus
}
#endif

#endif
