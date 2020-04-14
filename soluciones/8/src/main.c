#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"
#include "reactor.h"

#define GPIO_BUTTON 14
#define GPIO_PIR 12
#define GPIO_ALARM 2

fsm_t* fsm_new_alarm (int* validp, int pir, int alarm);
fsm_t* fsm_new_code (int* validp, int button);

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

static int valid_code = 0;
static fsm_t* code_fsm;
static fsm_t* alarm_fsm;

static void
code_func (struct event_handler_t* this)
{
    static portTickType period =  250 /portTICK_RATE_MS;
    fsm_fire (code_fsm);
    this->next_activation += period;
}


static void
alarm_func (struct event_handler_t* this)
{
    static portTickType period =  250 /portTICK_RATE_MS;
    fsm_fire (alarm_fsm);
    this->next_activation += period;
}

static EventHandler eh_long;

void A(fsm_t* this) { vTaskDelay(100 /portTICK_RATE_MS); }
void B(fsm_t* this) { vTaskDelay(100 /portTICK_RATE_MS); }
void C(fsm_t* this) { 
    vTaskDelay(100 /portTICK_RATE_MS); 
    eh_long.next_activation += 250 /portTICK_RATE_MS; 
}

static void
long_func (struct event_handler_t* this)
{
    static portTickType period =  250 /portTICK_RATE_MS;
    static struct fsm_trans_t tt[] = {
        {0, always, 1, A },
        {1, always, 2, B },
        {2, always, 0, C },
        {-1, NULL, -1, NULL}
    };
    static fsm_t* fsm_algo = fsm_new (tt);
    fsm_fire (fsm_algo);
}


static void
reactor_run (void* ignore)
{
  code_fsm = fsm_new_code (&valid_code, GPIO_BUTTON);
  alarm_fsm = fsm_new_alarm (&valid_code, GPIO_PIR, GPIO_ALARM);

  EventHandler eh_alarm;
  EventHandler eh_code;
  reactor_init ();

  event_handler_init (&eh_alarm, 1, alarm_func);
  reactor_add_handler (&eh_alarm);
  event_handler_init (&eh_code, 2, code_func);
  reactor_add_handler (&eh_code);
  event_handler_init (&eh_long, 3, long_func);
  reactor_add_handler (&eh_long);

  while (1) {
    reactor_handle_events ();
  }

}

void
user_init (void)
{
    xTaskHandle task_reactor;
    xTaskCreate (reactor_run, "reactor", 2048, NULL, 1, &task_reactor);
}
