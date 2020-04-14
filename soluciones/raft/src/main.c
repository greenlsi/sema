#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "gpio.h"
#include "freertos/task.h"
#include "fsm.h"

void gpio_config (GPIO_ConfigTypeDef *pGPIOConfig);
#define ETS_GPIO_INTR_ENABLE() _xt_isr_unmask ((1 << ETS_GPIO_INUM))

#define GPIO_PIR          12
#define GPIO_BUTTON_CODE  14
#define GPIO_BUTTON_LIGHT 13
#define GPIO_ALARM         2
#define GPIO_LIGHT         3

fsm_t* fsm_new_code (int* validp, volatile int* button);
fsm_t* fsm_new_alarm (int* validp, int pir, int alarm);
fsm_t* fsm_new_light (volatile int* button, int light);

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
domotic_control (void* ignore)
{
    gpio_setup (GPIO_Mode_Input, GPIO_PIN_INTR_DISABLE, GPIO_PullUp_EN,
                (1 << GPIO_PIR));
    gpio_setup (GPIO_Mode_Input, GPIO_PIN_INTR_NEGEDGE, GPIO_PullUp_EN,
                (1 << GPIO_BUTTON_CODE) | (1 << GPIO_BUTTON_LIGHT));
    gpio_setup (GPIO_Mode_Output, GPIO_PIN_INTR_DISABLE, GPIO_PullUp_DIS,
                (1 << GPIO_ALARM) | (1 << GPIO_LIGHT));
    gpio_intr_handler_register ((void *) io_intr_handler, NULL);
    ETS_GPIO_INTR_ENABLE();

    int valid_code = 0;
    fsm_t* code_fsm = fsm_new_code (&valid_code, &gpio_activated[GPIO_BUTTON_CODE]);
    fsm_t* alarm_fsm = fsm_new_alarm (&valid_code, GPIO_PIR, GPIO_ALARM);
    fsm_t* light_fsm = fsm_new_light (&gpio_activated[GPIO_BUTTON_LIGHT], GPIO_ALARM);

    portTickType period =  250 /portTICK_RATE_MS;
    portTickType last = xTaskGetTickCount();
    while (1) {
        fsm_fire (code_fsm);
        fsm_fire (alarm_fsm);
        fsm_fire (light_fsm);
        vTaskDelayUntil (&last, period);
    }
}

void
user_init (void)
{
    xTaskHandle task;
    xTaskCreate (domotic_control, "domo", 2048, NULL, 1, &task);
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
