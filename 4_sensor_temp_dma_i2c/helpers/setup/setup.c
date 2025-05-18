#include "setup.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "../display_oled/display_oled.h"
#include "../temperature_dma_adc/temperature_dma_adc.h"

void setup()
{
    stdio_init_all();
    sleep_ms(1000);

    setup_temperature_dma();
    sleep_ms(100);

    setup_display();
    sleep_ms(100);

    printf("Configuração Finalizada!\n");
}