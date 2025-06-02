#include <stdio.h>
#include "pico/stdlib.h"
#include "neopixel_driver.h"
#include "testes_cores.h"
#include "efeitos.h"
#include "efeito_curva_ar.h"
#include "numeros_neopixel.h"
#include <time.h>
#include <stdlib.h>
#include "pico/time.h" // Garante acesso a time_us_32()
#include "hardware/gpio.h" // Controle de GPIO


#define BOTAO_A 5 // Pino do botão
volatile int vezes = 0; // Quantidade de sorteios
volatile bool pressionou = false; // Flag para indicar se o botão foi pressionado

// Sorteia número inteiro entre [min, max]
int sorteia_entre(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

// Interrupção acionada ao pressionar o botão
void isr_botao(uint gpio, uint32_t events)
{
    if (gpio == BOTAO_A)
    {
        if (!pressionou)
        {
            vezes = sorteia_entre(100, 500); // Sorteia quantos números serão exibidos
            printf("Mostrando %d números aleatórios...\n", vezes);
            pressionou = true; // Marca que botão foi pressionado, evitando que: loop modifique a sua quantidade de vezes em quanto já foi apertado o botão/
        }
    }
}

// Inicializa o sistema e a matriz NeoPixel
void setup()
{
    stdio_init_all(); // Inicializa comunicação padrão
    sleep_ms(1000); // Aguarda conexão USB (opcional)

    // Inicializa botão
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A); // Ativa resistor pull-up
    sleep_ms(100); // estabilizar

    npInit(LED_PIN);     // Inicializa matriz NeoPixel
    srand(time_us_32()); // Semente para aleatoriedade

    // Habilita interrupção para o botão com callback
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &isr_botao);
}

// Exibe o número sorteado de 1 a 6
void mostrar_numero_sorteado(int numero)
{
    switch (numero)
    {
    case 1:
        mostrar_numero_1();
        break;
    case 2:
        mostrar_numero_2();
        break;
    case 3:
        mostrar_numero_3();
        break;
    case 4:
        mostrar_numero_4();
        break;
    case 5:
        mostrar_numero_5();
        break;
    case 6:
        mostrar_numero_6();
        break;
    }
}

int main()
{
    setup();  // Inicializa tudo

    while (1)
    {
        // Se botão foi pressionado, mostra os números sorteados
        if (pressionou)
        {
            for (int i = 0; i < vezes; i++)
            {
                int n = sorteia_entre(1, 6); // Sorteia número de 1 a 6
                printf("Número sorteado: %d\n", n);
                mostrar_numero_sorteado(n); // Mostra número no display
                sleep_ms(10);  // Pequena pausa entre números
            }
            pressionou = false; // Reseta a flag
        }
    }

    return 0;
}