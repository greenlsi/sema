#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"

static volatile int* GPIO_button;
static int GPIO_light;

#define TIMEOUT_MS 60000
static portTickType timeout_deadline;

static int
button_pressed (fsm_t* fsm)
{
    return *GPIO_button;
}

static int
timeout (fsm_t* fsm)
{
    return xTaskGetTickCount() >= timeout_deadline;
}

static void
turn_on (fsm_t* fsm)
{
    GPIO_OUTPUT_SET (GPIO_light, 0);
    *GPIO_button = 0;
    timeout_deadline = xTaskGetTickCount() + (TIMEOUT_MS /portTICK_RATE_MS);
}

static void
turn_off (fsm_t* fsm)
{
    GPIO_OUTPUT_SET (GPIO_light, 1);
}

fsm_t*
fsm_new_light (volatile int* button, int light)
{
    static fsm_trans_t light_tt[] = {
        {  0, button_pressed, 1, turn_on },
        {  1, button_pressed, 1, turn_on },
        {  1, timeout, 0, turn_off },
        { -1, NULL, -1, NULL },
    };
    GPIO_button = button;
    GPIO_light = light;
    return fsm_new (light_tt);
}
