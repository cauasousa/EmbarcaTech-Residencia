#ifndef SETUP_MICRO
#define SETUP_MICRO

// pico_stdlib: Inclui as bibliotecas padrão da Raspberry Pi Pico
// hardware_dma: Para utilizar a função Direct Memory Access (DMA)
// hardware_adc: Para leitura de ADC
// hardware_timer e hardware_clocks: Para configuração de frequências, temporização, etc.

// target_link_libraries(microphone_adc_example 
//     pico_stdlib 
//     hardware_dma
//     hardware_timer
//     hardware_adc
//     hardware_clocks
// )

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include <math.h> // Para sqrt()

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 200 // Número de amostras que serão feitas do ADC.
#define ADC_MAX 3.3f
#define ADC_MAX_VALUE 4095.0f


uint16_t read_adc_micro();
void micro_setup();

#endif
