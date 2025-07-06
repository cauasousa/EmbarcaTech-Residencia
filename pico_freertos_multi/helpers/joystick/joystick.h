#ifndef SETUP_JOYSTICK
#define SETUP_JOYSTICK

#include <stdio.h>        // Biblioteca padrão de entrada e saída
#include "hardware/adc.h" // Biblioteca para manipulação do ADC no RP2040
#include "pico/stdlib.h"  // Biblioteca padrão do Raspberry Pi Pico

// Define os ADC do Joystick
#define JOYSTICK_GPIO_0 26
#define JOYSTICK_GPIO_1 27

void joystick_setup(); // CONFIGURAR OS adc do joystick
float read_joystick_y(); // ler entrada y 
float read_joystick_x(); // ler entrada x

#endif