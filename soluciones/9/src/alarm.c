#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"

static int *valid_code = NULL;
static int GPIO_pir = 12;
static int GPIO_alarm = 2;

static int
code_ok (fsm_t* fsm)
{
    return *valid_code;
}

static int
presence (fsm_t* fsm)
{
    return ! GPIO_INPUT_GET (GPIO_pir);
}

static void
alarm_on (fsm_t* fsm)
{
    GPIO_OUTPUT_SET (GPIO_alarm, 0);
    puts ("ALARMA: INTRUSION !");
}

static void
turn_on (fsm_t* fsm)
{
    GPIO_OUTPUT_SET (GPIO_alarm, 1);
    *valid_code = 0;
    puts ("ALARMA: ARMADA");
}

static void
turn_off (fsm_t* fsm)
{
    GPIO_OUTPUT_SET (GPIO_alarm, 1);
    *valid_code = 0;
    puts ("ALARMA: DESARMADA");
}

fsm_t*
fsm_new_alarm (int* validp, int pir, int alarm)
{
    static fsm_trans_t alarm_tt[] = {
        {  0, code_ok, 1, turn_on },
        {  1, code_ok, 0, turn_off },
        {  1, presence, 1, alarm_on },
        { -1, NULL, -1, NULL },
    };
    valid_code = validp;
    GPIO_pir = pir;
    GPIO_alarm = alarm;
    GPIO_OUTPUT_SET (GPIO_alarm, 1);
    return fsm_new (alarm_tt);
}
