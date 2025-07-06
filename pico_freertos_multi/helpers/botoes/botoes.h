#ifndef SETUP_BOTOES
#define SETUP_BOTOES

#include <stdio.h>
#include "pico/stdlib.h"

#define BOTAO_A 5
#define BOTAO_B 6
#define BOTAO_SW 22

void botoes_setup();
int read_button_a();
int read_button_b();
int read_button_sw();

#endif