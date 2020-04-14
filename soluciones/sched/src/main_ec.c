#if 0

#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"
#include "screen.h"

fsm_t* fsm_new_ball (int deltax, int deltay);
fsm_t* fsm_new_racket (int column, int* button_up, int* button_down);
fsm_t* fsm_new_screen (void);


#define GPIO_UP   12
#define GPIO_DOWN 14

void gpio_config (GPIO_ConfigTypeDef *pGPIOConfig);
#define ETS_GPIO_INTR_ENABLE() _xt_isr_unmask ((1 << ETS_GPIO_INUM))

static volatile int gpio_activated[16];

static void
io_intr_handler (void)
{
    static portTickType debounce_timeout[16];
    portTickType now = xTaskGetTickCount();
    uint32 status = GPIO_REG_READ (GPIO_STATUS_ADDRESS);

    int i;
    for (i = 0; i < 16; ++i) {
        if ((status & (1 << i)) && (now >= debounce_timeout[i])) {
            debounce_timeout[i] = now + (200 /portTICK_RATE_MS);
            gpio_activated[i] = 1;
        }
    }
    /* rearm interrupts */
    GPIO_REG_WRITE (GPIO_STATUS_W1TC_ADDRESS, status);
}

static void
gpio_setup (GPIOMode_TypeDef mode, GPIO_INT_TYPE intrtype,
            GPIO_Pullup_IF pullup, uint16 mask)
{
    GPIO_ConfigTypeDef io_in_conf;
    io_in_conf.GPIO_Mode = mode;
    io_in_conf.GPIO_IntrType = intrtype;
    io_in_conf.GPIO_Pullup = pullup;
    io_in_conf.GPIO_Pin = mask;
    gpio_config (&io_in_conf);
}



static void
game (void* ignore)
{
    fsm_t* ball_fsm = fsm_new_ball (2, 1);
    fsm_t* racket_fsm = fsm_new_racket (2, &gpio_activated[GPIO_UP], &gpio_activated[GPIO_DOWN]);
    fsm_t* screen_fsm = fsm_new_screen ();

    portTickType period =  20 /portTICK_RATE_MS;
    portTickType last = xTaskGetTickCount();
    while (1) {
        fsm_fire (ball_fsm);
        fsm_fire (racket_fsm);
        fsm_fire (screen_fsm);
        vTaskDelayUntil (&last, period);
    }
}

void
user_init (void)
{
    gpio_setup (GPIO_Mode_Input, GPIO_PIN_INTR_NEGEDGE, GPIO_PullUp_EN,
                (1 << GPIO_UP) | (1 << GPIO_DOWN));
    gpio_intr_handler_register ((void *) io_intr_handler, NULL);
    ETS_GPIO_INTR_ENABLE();

    screen_setup (0);

    xTaskHandle task;
    xTaskCreate (game, "game", 2048, NULL, 2, &task);
}

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

#endif
