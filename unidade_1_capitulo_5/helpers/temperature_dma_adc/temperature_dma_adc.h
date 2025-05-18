#ifndef SETUP_TEMPERATURE_DMA_ADC
#define SETUP_TEMPERATURE_DMA_ADC

#include "hardware/adc.h" // Controle do ADC interno
#include "hardware/dma.h" // Controle do DMA


#define NUM_SAMPLES 100 // Número de amostras por ciclo de leitura

uint16_t adc_buffer[NUM_SAMPLES]; // Buffer para armazenar as amostras do ADC

extern int dma_chan;
extern dma_channel_config cfg;

void setup_temperature_dma(void); // configurações iniciais
float read_temperature(void);

#endif
