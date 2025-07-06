#include "micro.h"

uint16_t read_adc_micro()
{
    adc_select_input(MIC_CHANNEL);

    uint32_t soma = 0;
    uint16_t adc_buffer[SAMPLES];

    // Coleta de amostras
    for (int i = 0; i < SAMPLES; i++)
    {
        adc_buffer[i] = adc_read();
        soma += adc_buffer[i];
        sleep_us(5);
    }

    uint32_t average = soma / SAMPLES;

    // Cálculo da variação (mais sensível ao som)
    uint32_t variation_value = 0;
    for (int i = 0; i < SAMPLES; i++)
    {
        int32_t delta = (int32_t)adc_buffer[i] - (int32_t)average;
        variation_value += (delta > 0) ? delta : -delta; // pegandos os picos
    }

    return variation_value / SAMPLES;
}

void micro_setup()
{
    // printf("Configurando Micro...\n");

    adc_init(); // já tem no joystick
    adc_gpio_init(MIC_PIN);

    adc_select_input(MIC_CHANNEL);

    adc_set_clkdiv(ADC_CLOCK_DIV);

    // printf("Microfone pronto.\n");
}
