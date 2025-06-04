#ifndef SETUP_TEMPERATURE_DMA_ADC
#define SETUP_TEMPERATURE_DMA_ADC

#include "hardware/adc.h" // Controle do ADC interno


extern volatile float current_temp; // Temperatura atual

void setup_temperature(void); // configurações iniciais
void read_temperature(void); // ler a temperatura

#endif