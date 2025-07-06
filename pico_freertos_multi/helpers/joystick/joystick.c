#include "joystick.h"


float read_joystick(int select)
{
    const float VREF = 3.33f;
    const uint16_t ADC_MAX = 4095;

    adc_select_input(select);
    uint16_t leitura = adc_read();
    float tensao = (leitura * VREF) / ADC_MAX;

    return tensao;
}

// posicao no eixo y
float read_joystick_x()
{
    return read_joystick(0);
}

// posicao no eixo y
float read_joystick_y()
{
    return read_joystick(1);
}

void joystick_setup()
{

    adc_init();
    sleep_ms(50); // Aguarda estabilização

    adc_gpio_init(JOYSTICK_GPIO_0); // GPIO26 = ADC0
    sleep_ms(50);                   // Aguarda estabilização

    adc_gpio_init(JOYSTICK_GPIO_1); // GPIO27 = ADC1
    sleep_ms(50);                   // Aguarda estabilização
}
