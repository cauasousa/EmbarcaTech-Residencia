#include "leds.h"
#include "pico/stdlib.h"
#include <stdio.h>

void turn_on_led(int led_red, int led_green, int led_blue) {
    // Ativa ou desativa os LEDs conforme parâmetros
    gpio_put(LED_RED, led_red);
    gpio_put(LED_GREEN, led_green);
    gpio_put(LED_BLUE, led_blue);
}


// CONFIGURAR OS LEDS
void leds_setup() {
    
    // Configura o LED no pino 13
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_pull_up(LED_RED); // (nível alto em pull-up)
    gpio_put(LED_RED, 0); // Começa desligado 

    // Configura o LED no pino 11
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_pull_up(LED_GREEN);// (nível alto em pull-up)
    gpio_put(LED_GREEN, 0); // Começa desligado 

    // Configura o LED no pino 12
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_pull_up(LED_BLUE); // (nível alto em pull-up)
    gpio_put(LED_BLUE, 0); // Começa desligado 
}