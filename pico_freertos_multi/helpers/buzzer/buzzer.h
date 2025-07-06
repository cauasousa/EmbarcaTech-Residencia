#ifndef SETUP_BUZZER
#define SETUP_BUZZER

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Configuração do pino do buzzer
#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

void buzzer_setup();
void touch_buzzer();

#endif