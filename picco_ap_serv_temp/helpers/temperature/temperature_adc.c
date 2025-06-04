#include <stdio.h>
#include "pico/stdlib.h" // Funções básicas do SDK Pico
#include "temperature_adc.h"


// Função para medir e armazenar a temperatura
void read_temperature(void)
{
    adc_select_input(4);
    uint32_t raw32 = adc_read();
    const uint32_t bits = 12;

    // Converter leitura bruta para um valor de 16 bits
    uint16_t raw16 = raw32 << (16 - bits) | raw32 >> (2 * bits - 16);

    // Converter leitura para temperatura em Celsius
    const float conversion_factor = 3.3 / (65535);
    float reading = raw16 * conversion_factor;

    // Aplicar equação de conversão
    float deg_c = 27 - (reading - 0.706) / 0.001721;
    current_temp = deg_c;
    printf("Temperatura medida: %.2f °C\n", deg_c);
}

void setup_temperature()
{
    // Inicializa o ADC e habilita o sensor de temperatura interno
    adc_init();
    adc_select_input(4); // Canal 4 é o sensor de temperatura interna do RP2040
    adc_set_temp_sensor_enabled(true);
}