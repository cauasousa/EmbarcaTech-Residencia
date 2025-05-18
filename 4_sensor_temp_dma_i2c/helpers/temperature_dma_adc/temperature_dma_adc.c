#include <stdio.h>
#include "pico/stdlib.h"  // Funções básicas do SDK Pico
#include "temperature_dma_adc.h"

uint16_t adc_buffer[NUM_SAMPLES]; // Buffer para armazenar as amostras do ADC

int dma_chan;
dma_channel_config cfg;

// Converte o valor bruto do ADC (12 bits) para temperatura em graus Celsius
float convert_to_celsius(uint16_t raw)
{
    const float conversion_factor = 3.3f / (1 << 12); // Fator de conversão para 3.3V e 12 bits
    float voltage = raw * conversion_factor;          // Converte valor para tensão
    return 27.0f - (voltage - 0.706f) / 0.001721f;    // Fórmula do datasheet do RP2040
}

float read_temperature()
{
    // Garante que o FIFO esteja vazio antes de começar
    adc_fifo_drain();
    // Configura o ADC para colocar dados no FIFO
    adc_run(false); // Desliga ADC temporariamente

    adc_fifo_setup(
        true, // Envia dados para o FIFO
        true, // Habilita DMA para o FIFO
        1,    // Gatilho a cada amostra
        false,
        false);

    adc_run(true); // Liga ADC para começar a amostrar

    // Inicia a transferência DMA: do FIFO ADC para adc_buffer
    dma_channel_configure(
        dma_chan,
        &cfg,
        adc_buffer,    // Endereço de destino na RAM
        &adc_hw->fifo, // Endereço de origem (registrador FIFO do ADC)
        NUM_SAMPLES,   // Número de transferências (amostras)
        true           // Inicia imediatamente
    );

    // Aguarda até que a transferência DMA seja concluída
    dma_channel_wait_for_finish_blocking(dma_chan);

    // Desliga o ADC após capturar os dados
    adc_run(false);

    // Calcula a média das temperaturas lidas
    float sum = 0.0f;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += convert_to_celsius(adc_buffer[i]); // Converte cada valor para °C e soma
    }

    float avg_temp = sum / NUM_SAMPLES; // Temperatura média em °C

    return avg_temp;
}

void setup_temperature_dma()
{
    // Inicializa o ADC e habilita o sensor de temperatura interno
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4); // Canal 4 é o sensor de temperatura interna do RP2040

    // Configura o canal DMA para receber dados do ADC
    dma_chan = dma_claim_unused_channel(true);                     // Requisita um canal DMA disponível
    cfg = dma_channel_get_default_config(dma_chan); // Obtem configuração padrão

    // Configurações do canal DMA
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16); // Cada leitura é de 16 bits
    channel_config_set_read_increment(&cfg, false);           // Endereço fixo (registrador ADC FIFO)
    channel_config_set_write_increment(&cfg, true);           // Incrementa para armazenar em adc_buffer[]
    channel_config_set_dreq(&cfg, DREQ_ADC);                  // Dispara automaticamente com dados do ADC
}