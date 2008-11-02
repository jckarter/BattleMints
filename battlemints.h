#ifndef __BATTLEMINTS_H__
#define __BATTLEMINTS_H__

#ifdef __cplusplus
extern "C" {
#endif

void battlemints_start(void);

void battlemints_tick(void);
void battlemints_draw(void);

void battlemints_input(int player, float x, float y, int buttons);

void battlemints_finish(void);

#ifdef __cplusplus
}
#endif

#endif
