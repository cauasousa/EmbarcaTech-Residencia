#include "botoes.h"

int read_button_a(){
    return gpio_get(BOTAO_A);
}

int read_button_b(){
    return gpio_get(BOTAO_A);
}

int read_button_sw(){
    return gpio_get(BOTAO_A);
}

void botoes_setup()
{
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
    sleep_ms(50);                  // Aguarda estabilização

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    sleep_ms(50);                  // Aguarda estabilização

    gpio_init(BOTAO_SW);
    gpio_set_dir(BOTAO_SW, GPIO_IN);
    gpio_pull_up(BOTAO_SW);
    sleep_ms(50);                  // Aguarda estabilização
    
    
    // gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    // gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);
}

