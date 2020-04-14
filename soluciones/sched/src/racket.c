#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"
#include "screen.h"

static int max_x;
static int max_y;

struct fsm_racket_t {
    fsm_t fsm;
    int x;
    int y;
    int size;
    int* up;
    int* down;
};
typedef struct fsm_racket_t fsm_racket_t;

static void draw (fsm_racket_t* racket) {
  int i;
  for (i = 0; i < racket->size; ++i)
    screen_printxy (racket->x, racket->y + i, "|");
}

static void undraw (fsm_racket_t* racket) {
  int i;
  for (i = 0; i < racket->size; ++i)
    screen_printxy (racket->x, racket->y + i, " ");
}

static int button_up (fsm_t* fsm) {
  fsm_racket_t* racket = (fsm_racket_t*) fsm;
  return *racket->up && (racket->y > 0);
}

static int button_down (fsm_t* fsm) {
  fsm_racket_t* racket = (fsm_racket_t*) fsm;
  return *racket->down && (racket->y + racket->size <= max_y);
}

static void go_up (fsm_t* fsm) {
  fsm_racket_t* racket = (fsm_racket_t*) fsm;
  undraw (racket);
  racket->y --;
  draw (racket);
  *racket->up = 0;
}

static void go_down (fsm_t* fsm) {
  fsm_racket_t* racket = (fsm_racket_t*) fsm;
  undraw (racket);
  racket->y ++;
  draw (racket);
  *racket->down = 0;
}

fsm_t*
fsm_new_racket (int column, int* b_up, int* b_down)
{
    static fsm_trans_t racket_tt[] = {
        {  0, button_up,   0, go_up   },
        {  0, button_down, 0, go_down },
        { -1, NULL, -1, NULL },
    };
    fsm_racket_t* racket = (fsm_racket_t*) malloc (sizeof (fsm_racket_t));
    fsm_init ((fsm_t*) racket, racket_tt);
    racket->x = column;
    racket->y = max_y / 2;
    racket->size = 3;
    racket->up = b_up;
    racket->down = b_down;
    screen_size (&max_x, &max_y);
    return (fsm_t*) racket;
}
