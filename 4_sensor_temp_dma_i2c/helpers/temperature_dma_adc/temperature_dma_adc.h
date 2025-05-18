#ifndef SETUP_TEMPERATURE_DMA_ADC
#define SETUP_TEMPERATURE_DMA_ADC

#include "hardware/adc.h" // Controle do ADC interno
#include "hardware/dma.h" // Controle do DMA


#define NUM_SAMPLES 100 // Número de amostras por ciclo de leitura

void setup_temperature_dma(void); // configurações iniciais
float read_temperature(void);

#endif
