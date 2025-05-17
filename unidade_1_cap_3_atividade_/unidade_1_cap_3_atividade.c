#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "neopixel.c"

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

#define ADC_CLOCK_DIV 96.f

#define LED_PIN 7
#define LED_COUNT 25

#define DETECTION_THRESHOLD 10

#define abs(x) ((x < 0) ? (-x) : (x))
uint16_t adc_value = 0;

bool mic_power(repeating_timer_t *t)
{
    adc_select_input(MIC_CHANNEL);

    uint32_t soma = 0;
    int tam = 200;
    uint16_t samples[tam];

    // Coleta de amostras
    for (int i = 0; i < tam; i++)
    {
        samples[i] = adc_read();
        soma += samples[i];
        sleep_us(5);  
    }

    uint32_t average  = soma / tam;

    // Cálculo da variação (mais sensível ao som)
    uint32_t variation_value  = 0;
    for (int i = 0; i < tam; i++)
    {
        int32_t delta = (int32_t)samples[i] - (int32_t)average;
        variation_value  += (delta > 0) ? delta : -delta;  // pegandos os picos
    }

    adc_value = variation_value  / tam;  

    printf("\nValor ajustado do adc: %d", adc_value);
    return true;
}


int main()
{
    stdio_init_all();
    sleep_ms(5000);

    printf("Inicializando NeoPixel...\n");
    npInit(LED_PIN, LED_COUNT);

    printf("Configurando ADC...\n");
    adc_gpio_init(MIC_PIN);
    adc_init();
    adc_select_input(MIC_CHANNEL);
    adc_set_clkdiv(ADC_CLOCK_DIV);
    printf("ADC pronto.\n");

    static repeating_timer_t timer;
    add_repeating_timer_ms(800, mic_power, NULL, &timer);

    printf("Sistema iniciado.\n");

    while (true)
    {

        if (adc_value > DETECTION_THRESHOLD)
        {
            // Ativa padrão de luz
            npClear();
            for (int i = 0; i < LED_COUNT; i++)
            {
                npSetLED(i, 30, 200, 80);
            }
            npWrite();
        }
        else
        {
            // Desliga os LEDs
            npClear();
            npWrite();
        }

        tight_loop_contents(); // Aguarda interrupções
        sleep_ms(100);
    }
}
