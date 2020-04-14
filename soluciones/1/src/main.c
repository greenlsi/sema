#include <string.h>
#include <stdio.h>
#include "esp_common.h"
#include "freertos/task.h"


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

static void
pascal (void* arg)
{
    int i, j;
    int rows = (int) arg;
    int *buf;
    int **pnumbers;

    int n = 0;
    for (i = 0; i < rows; ++i)
        n += (i + 1);

    buf = (int *) malloc (n * sizeof (int));
    /* este segundo malloc no haría falta para un número fijo de filas */
    pnumbers = (int **) malloc (rows * sizeof (int *));

    pnumbers[0] = buf;
    pnumbers[0][0] = 1;
    for (i = 1; i < rows; ++i) {
        pnumbers[i] = pnumbers[i - 1] + i;
        pnumbers[i][0] = pnumbers[i][i] = 1;
        for (j = 1; j < i; ++j) {
            pnumbers[i][j] = pnumbers[i-1][j-1] + pnumbers[i-1][j];
        }

    }

    for (i = 0; i < rows; i++) {
        for (j = 0; j <= i; j++) {
            printf("%d ", pnumbers[i][j]);
        }
        printf("\n");
    }

    free (pnumbers);
    free (buf);
}

void user_init(void)
{
    xTaskHandle task_pascal;
    xTaskCreate (pascal, "startup", 2048, (void*) 7, 1, &task_pascal);
}
