#include <stdio.h>
#include "pico/stdlib.h"

// Inclusão das bibliotecas auxiliares
#include "inc/ssd1306.h"
#include "helpers/setup/setup.h"
#include "helpers/display_oled/display_oled.h"
#include "helpers/temperature_dma_adc/temperature_dma_adc.h"

// Variável para armazenar a temperatura
float temp = 0.0f;

// Função chamada a cada segundo para atualizar o display
bool print_display(struct repeating_timer *t)
{
    temp = read_temperature(); // Lê a temperatura do sensor

    char temp_text[20];
    snprintf(temp_text, sizeof(temp_text), "Temp: %.1f C", temp); // Formata o texto da temperatura
    char *text[] = {temp_text};                                   // Cria um array de strings para enviar ao display

    write_display(text); // Escreve no display OLED

    return true; // Retorna verdadeiro para continuar repetindo
}

int main()
{
    setup(); // Inicializa os periféricos (display, ADC, DMA.)

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, print_display, NULL, &timer); // Chama print_display a cada 1000 ms

    while (true)
    {
        tight_loop_contents();
    }
}