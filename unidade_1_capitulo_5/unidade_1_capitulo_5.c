#include <stdio.h>
#include "pico/stdlib.h"
// #include "hardware/i2c.h"
// #include "hardware/adc.h" // Controle do ADC interno
// #include "hardware/dma.h" // Controle do DMA

#include "inc/ssd1306.h"
#include "helpers/setup/setup.h"
#include "helpers/display_oled/display_oled.h"
#include "helpers/temperature_dma_adc/temperature_dma_adc.h"

int main()
{
    setup();
    
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
