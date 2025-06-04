#include "led.h"
#include "pico/stdlib.h"
#include <stdio.h>

void turn_on_led(int led_value)
{
    // Ativa ou desativa os LEDs conforme parâmetros
    gpio_put(LED_PIN, led_value);
}

// CONFIGURAR OS LEDS
void led_setup()
{
    // Configura o LED no pino 13
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_pull_up(LED_PIN); // (nível alto em pull-up)
    gpio_put(LED_PIN, 0);  // Começa desligado
}