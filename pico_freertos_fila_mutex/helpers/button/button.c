#include "button.h"


int read_button_sw(){
    return !gpio_get(BOTAO_SW);
}

void button_setup()
{                  

    gpio_init(BOTAO_SW);
    gpio_set_dir(BOTAO_SW, GPIO_IN);
    gpio_pull_up(BOTAO_SW);
    
}

