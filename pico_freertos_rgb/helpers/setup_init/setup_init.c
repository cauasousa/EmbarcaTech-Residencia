#include "setup_init.h" // importação

void setup_init()
{
    // gpio_put(LED_BLUE, 1);
    stdio_init_all(); // Inicializa USB serial
    sleep_ms(2000);

    gpio_init(LED_RED);              // Inicializa o pino como GPIO
    gpio_set_dir(LED_RED, GPIO_OUT); // Configura o pino como saída digital
    gpio_put(LED_RED, 0);            // começa desligado

    gpio_init(LED_GREEN);              // Inicializa o pino como GPIO
    gpio_set_dir(LED_GREEN, GPIO_OUT); // Configura o pino como saída digital
    gpio_put(LED_GREEN, 0);            // começa desligado

    gpio_init(LED_BLUE);              // Inicializa o pino como GPIO
    gpio_set_dir(LED_BLUE, GPIO_OUT); // Configura o pino como saída digital
    gpio_put(LED_BLUE, 0);            // começa desligado
}
