#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"
#include "screen.h"

static int max_x;
static int max_y;

struct fsm_ball_t {
    fsm_t fsm;
    int x;
    int y;
    int speed_x;
    int speed_y;
};
typedef struct fsm_ball_t fsm_ball_t;

static void draw (fsm_ball_t* ball) {
  screen_printxy (ball->x, ball->y, "*");
}

static void undraw (fsm_ball_t* ball) {
  screen_printxy (ball->x, ball->y, " ");
}

static int top_hit (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  return ball->y + ball->speed_y < 0;
}

static int bottom_hit (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  return ball->y + ball->speed_y > max_y;
}

static int left_hit (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  return ball->x + ball->speed_x < 0;
}

static int right_hit (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  return ball->x + ball->speed_x > max_x;
}

static int clean_path (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  if (top_hit(fsm) || bottom_hit(fsm) || right_hit(fsm) || left_hit(fsm))
      return 0;
  return screen_getxy(ball->x + ball->speed_x, ball->y + ball->speed_y) == ' ';
}

static void bounce_y (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  ball->speed_y = - ball->speed_y;
}

static void bounce_x (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  ball->speed_x = - ball->speed_x;
}

static void advance (fsm_t* fsm) {
  fsm_ball_t* ball = (fsm_ball_t*) fsm;
  undraw (ball);
  ball->x += ball->speed_x;
  ball->y += ball->speed_y;
  draw (ball);
}

fsm_t*
fsm_new_ball (int deltax, int deltay)
{
    static fsm_trans_t ball_tt[] = {
        {  0, top_hit,    0, bounce_y },
        {  0, bottom_hit, 0, bounce_y },
        {  0, right_hit,  0, bounce_x },
        {  0, left_hit,   0, bounce_x },
        {  0, clean_path, 0, advance  },
        { -1, NULL, -1, NULL },
    };
    fsm_ball_t* ball = (fsm_ball_t*) malloc (sizeof (fsm_ball_t));
    fsm_init ((fsm_t*) ball, ball_tt);
    ball->x = max_x / 2;
    ball->y = max_y / 2;
    ball->speed_x = deltax;
    ball->speed_y = deltay;
    screen_size (&max_x, &max_y);
    return (fsm_t*) ball;
}
