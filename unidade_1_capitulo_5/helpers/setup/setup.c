#include "setup.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "../display_oled/display_oled.h"

void setup()
{
    stdio_init_all();
    sleep_ms(1000);

    setup_display();

    printf("Configuração Finalizada!\n");
}