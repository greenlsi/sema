#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "screen.h"
#include "esp_common.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "fsm.h"

static
void
refresh_screen (void* arg)
{
  portTickType period =  250 /portTICK_RATE_MS;
  portTickType last = xTaskGetTickCount();
  while (1) {
      screen_refresh ();
      vTaskDelayUntil (&last, period);
  }
}

static struct termios oldtc, newtc;
static char* screen;
static int columns;
static int lines;

static xTaskHandle t_screen;
static xSemaphoreHandle m_scr;

static char*
scr (int x, int y)
{
  if (x >= columns)
    x = columns - 1;
  if (y >= lines)
    y = lines - 1;
  return screen + y * (columns + 1) + x;
}

void
screen_setup (int prio)
{
  columns = 80;
  lines = 24;

  m_scr = xSemaphoreCreateMutex();
  screen = (char *) malloc ((columns + 1) * lines);
  screen_clear ();

  printf ("\e[2J\e[%d;1f", lines + 1);
  fflush (stdout);

  if (prio > 0)
    xTaskCreate (refresh_screen, "screen", 1024, NULL, prio, &t_screen);
}

void screen_size (int* maxx, int* maxy)
{
  *maxx = columns - 1;
  *maxy = lines - 1;
}

void
screen_refresh (void)
{
  int y;

  printf ("\e7\e[?25l"); // save cursor, hide cursor

  xSemaphoreTake (m_scr, portMAX_DELAY);
  for (y = 0; y < lines; ++y) {
    printf ("\e[%d;1f%s", y+1, scr(0,y));
  }
  xSemaphoreGive (m_scr);

  printf ("\e8\e[?25h"); // show cursor, restore cursor position
  fflush (stdout);
}

void
screen_clear (void)
{
  int y;

  xSemaphoreTake (m_scr, portMAX_DELAY);
  memset (screen, ' ', (columns + 1) * lines);
  for (y = 0; y < lines; ++y)
    *scr(columns, y) = '\0';
  xSemaphoreGive (m_scr);
}

void
screen_printxy (int x, int y, const char* txt)
{
  char* p = scr(x,y);
  xSemaphoreTake (m_scr, portMAX_DELAY);
  while (*txt) {
    *p++ = *txt++;
  }
  xSemaphoreGive (m_scr);
}

char screen_getxy (int x, int y)
{
  return *scr(x,y);
}

static int  always (fsm_t* this) { return 1; }
static void refresh (fsm_t* this) { screen_refresh(); }

fsm_t*
fsm_new_screen (void)
{
    static fsm_trans_t screen_tt[] = {
        {  0, always, 0, refresh },
        { -1, NULL, -1, NULL },
    };
    return fsm_new (screen_tt);
}
