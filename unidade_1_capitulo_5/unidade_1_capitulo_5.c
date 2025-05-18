#include <stdio.h>
#include "pico/stdlib.h"

#include "inc/ssd1306.h"
#include "helpers/setup/setup.h"
#include "helpers/display_oled/display_oled.h"
#include "helpers/temperature_dma_adc/temperature_dma_adc.h"

float temp = 0.0f;

bool print_display(struct repeating_timer *t)
{
    temp = read_temperature();

    char temp_text[20];
    snprintf(temp_text, sizeof(temp_text), "Temp: %.1f C", temp);
    char *text[] = {temp_text};

    write_display(text);

    return true;
}

int main()
{
    setup();

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, print_display, NULL, &timer);

    while (true)
    {
        tight_loop_contents();
    }
}
