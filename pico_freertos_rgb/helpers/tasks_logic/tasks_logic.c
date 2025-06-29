#include "tasks_logic.h"  // Importa definições relacionadas

// Liga os LEDs conforme os valores passados (1 = ligado, 0 = desligado)
void turn_on_leds(int red, int green, int blue)
{
    gpio_put(LED_RED, red);
    gpio_put(LED_GREEN, green);
    gpio_put(LED_BLUE, blue);
}

// Task3: acende o LED vermelho por 5 segundos, depois é acionado o led verde após passar 5segundos
void task3(void *parameter)
{
    while (1)
    {
        printf("Task1: RED\n");
        turn_on_leds(1, 0, 0); // Liga apenas o vermelho
        vTaskDelay(pdMS_TO_TICKS(13000)); // Espera 13 segundos
    }
}

// Task2: acende o LED verde após 5s de espera, e espera por 13s (5000+8000)após (tempo q é acionado do vermelho + amarelo)
void task2(void *parameter)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000)); // Espera 5 segundos
        printf("Task2: VERDE\n");
        turn_on_leds(0, 1, 0); // Liga apenas o verde
        vTaskDelay(pdMS_TO_TICKS(8000)); // Espera 8 segundos
    }
}

// Task1: acende o LED amarelo (vermelho + verde) após 10s (tempo que o vermelho e verde são acionados individualmente), e mantém por 3s
void task1(void *parameter)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000)); // Espera 10 segundos
        printf("Task2: AMARELO\n");
        turn_on_leds(1, 1, 0); // Liga vermelho e verde = amarelo
        vTaskDelay(pdMS_TO_TICKS(3000)); // Espera 3 segundos
    }
}
