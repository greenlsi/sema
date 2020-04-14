#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"

static int secret[] = { 1, 1, 1 };
static int code[10];
static int current = 0;
static portTickType digit_deadline = 0;

static int *valid_code = NULL;
static volatile int* GPIO_button;

static int started = 0;

static int
code_ok (fsm_t* fsm)
{
    return current == (sizeof(secret) / sizeof(secret[0]));
}

static int
timeout (fsm_t* fsm)
{
    return !code_ok(fsm) && started && (xTaskGetTickCount() > digit_deadline);
}

static int
timeout_and_digit_ok (fsm_t* fsm)
{
    return timeout(fsm) && (code[current] == secret[current]);
}

static int
timeout_and_digit_not_ok (fsm_t* fsm)
{
    return timeout(fsm) && (code[current] != secret[current]);
}

static int
button_pressed (fsm_t* fsm)
{
    return !code_ok(fsm) && !timeout(fsm) && *GPIO_button;
}

static void
increment_current_digit (fsm_t* fsm)
{
    code[current] ++;
    digit_deadline = xTaskGetTickCount() + (1000 / portTICK_RATE_MS);
    started = 1;
    *GPIO_button = 0;
}

static void
next_digit (fsm_t* fsm)
{
    code[++current] = 0;
    started = 0;
}

static void
reset (fsm_t* fsm)
{
    printf ("CODIGO: RESET %d (%d/%d, %d/%d, %d/%d)\n", current,
      code[0], secret[0], code[1], secret[1], code[2], secret[2]);
    current = 0;
    code[current] = 0;
    started = 0;
}

static void
accept_code (fsm_t* fsm)
{
    *valid_code = 1;
    reset (fsm);
    puts ("CODIGO: CORRECTO");
}

fsm_t*
fsm_new_code (int* validp, volatile int* button)
{
    static fsm_trans_t code_tt[] = {
        {  0, code_ok, 0, accept_code },
        {  0, timeout_and_digit_ok, 0, next_digit },
        {  0, timeout_and_digit_not_ok, 0, reset },
        {  0, button_pressed, 0, increment_current_digit },
        { -1, NULL, -1, NULL },
    };
    valid_code = validp;
    GPIO_button = button;
    return fsm_new (code_tt);
}
