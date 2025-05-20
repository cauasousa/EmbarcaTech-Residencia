#include "setup.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "../display_oled/display_oled.h"
#include "../leds/leds.h"
#include "../buzzer/buzzer.h"

void setup()
{
    // Inicia as funções padrão de entrada/saída
    stdio_init_all();
    sleep_ms(1000); // Espera um pouco pra garantir que tudo inicializou

    // Inicializa os LEDs
    leds_setup();
    sleep_ms(100);

    // Inicializa o display OLED
    setup_display();
    sleep_ms(100);

    // Inicializa o Buzzer  
    buzzer_setup();
    sleep_ms(100);

    printf("Configuração Finalizada!\n");
}