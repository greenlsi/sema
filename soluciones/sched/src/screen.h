#ifndef SCREEN_H
#define SCREEN_H

void screen_setup (int prio);
void screen_size (int* maxx, int* maxy);
void screen_refresh (void);

void screen_clear (void);
void screen_printxy (int x, int y, const char* txt);
char screen_getxy (int x, int y);

#endif
