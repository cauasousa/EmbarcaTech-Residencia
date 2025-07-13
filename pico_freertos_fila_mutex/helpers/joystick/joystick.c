#include "joystick.h"


float read_joystick(int select)
{
    adc_select_input(select);
    return adc_read();
}

// posicao no eixo y
float read_joystick_y()
{
    return read_joystick(0);
}

// posicao no eixo x
float read_joystick_x()
{
    return read_joystick(1);
}

void joystick_setup()
{

    adc_init();

    adc_gpio_init(JOYSTICK_GPIO_0); // GPIO26 = ADC0

    adc_gpio_init(JOYSTICK_GPIO_1); // GPIO27 = ADC1
}
