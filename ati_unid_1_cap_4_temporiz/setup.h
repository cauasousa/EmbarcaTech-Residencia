#ifndef SETUP_H
#define SETUP_H

#include <stdio.h>
#include "pico/stdlib.h"

#define LED_VERDE 11
#define LED_VERMELHO 13
#define BOTAO_A 5
#define BOTAO_B 6

// Configuração do pino do buzzer
#define BUZZER_PIN 21
// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

void setup(void); // configurações iniciais
int64_t beep(alarm_id_t id, void *user_data); // funcao para fazer o beep
int64_t beep_desativar(alarm_id_t id, void *user_data);
void gpio_callback(uint gpio, uint32_t events);  // Declaração da callback

#endif
