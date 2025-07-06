#include "setup.h"

void setup_init_sys()
{
    // Inicia as funções padrão de entrada/saída
    stdio_init_all();
    sleep_ms(100); // Espera um pouco pra garantir que tudo inicializou

    // Inicializa os LEDs
    leds_setup();
    sleep_ms(100); // Espera um pouco pra garantir que tudo inicializou
    
    // Inicializa o display OLED
    botoes_setup();
    sleep_ms(100); // Espera um pouco pra garantir que tudo inicializou
    
    // Inicializa o Buzzer  
    buzzer_setup();
    sleep_ms(100); // Espera um pouco pra garantir que tudo inicializou

    joystick_setup();
    sleep_ms(100); // Espera um pouco pra garantir que tudo inicializou
    
    micro_setup();
    sleep_ms(100); // Espera um pouco pra garantir que tudo inicializou
    
    printf("Configuração Finalizada!\n");
}