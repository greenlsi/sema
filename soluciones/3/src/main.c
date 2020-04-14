#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "freertos/task.h"
#include "gpio.h"

#define LED 2

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

static const char*
morse (int c)
{
    static const char* morse_ch[] = {
        ". -   ",     /* A */
        "- . . .   ", /* B */
        "- . - .   ", /* C */
        "- . .   ",   /* D */
        ".   ",       /* E */
        ". . - .   ", /* F */
        "- - .   ",   /* G */
        ". . . .   ", /* H */
        ". .   ",     /* I */
        ". - - -   ", /* J */
        "- . -   ",   /* K */
        ". - . .   ", /* L */
        "- -   ",     /* M */
        "- .   ",     /* N */
        "- - -   ",   /* O */
        ". - - .   ", /* P */
        "- - . -   ", /* Q */
        ". - .   ",   /* R */
        ". . .   ",   /* S */
        "-   ",       /* T */
        ". . -   ",   /* U */
        ". . . -   ", /* V */
        ". - -   ",   /* W */
        "- . . -   ", /* X */
        "- . - -   ", /* Y */
        "- - . .   ", /* Z */
    };
    if (c == ' ')
        return "     ";
    if ((c >= 'A') && (c <= 'Z'))
        c += 'a' - 'A';
    if ((c < 'a') || (c > 'z'))
        return "";
    return morse_ch[c - 'a'];
}

static char *
str2morse (char *buf , const char* str, int n)
{
    char *p = buf;
    while (*str) {
        const char* c = morse (*str);
        int len = strlen(c);
        strncpy (p, c, n);
        n -= len; p += len;
        ++str;
    }
    return buf;
}

static void
morse_send (const char* msg)
{
    while (*msg) {
        switch (*msg) {
        case '.':
            GPIO_OUTPUT_SET (LED, 1);
            vTaskDelay (250 /portTICK_RATE_MS);
            break;
        case '-':
            GPIO_OUTPUT_SET (LED, 1);
            vTaskDelay (3*250 /portTICK_RATE_MS);
            break;
        case ' ':
            vTaskDelay (250 /portTICK_RATE_MS);
            break;
        }
        GPIO_OUTPUT_SET (LED, 0);
        ++msg;
    }
}

static void
hello (void* ignore)
{
    char msg[256];
    str2morse (msg, "hola mundo ", 256);
    while(true) {
        puts ("hola mundo ");
        puts (msg);
        fflush (stdout);
        morse_send (msg);
    }
}

void user_init(void)
{
    xTaskHandle task_hello;
    xTaskCreate (hello, "hello", 2048, NULL, 1, &task_hello);
}
