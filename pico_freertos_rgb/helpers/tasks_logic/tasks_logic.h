#ifndef TASKS_LOGIC_H
#define TASKS_LOGIC_H

#include "pico/stdlib.h"
#include <stdio.h>
#include "tasks_logic.h"
#include "setup_init/setup_init.h"
#include "FreeRTOS.h"
#include "task.h"

// Task3: acende o LED vermelho por 5 segundos, depois é acionado o led verde após passar 5segundos
void task3(void *parameter);
// Task2: acende o LED verde após 5s de espera, e espera por 13s (5000+8000)após (tempo q é acionado do vermelho + amarelo)
void task2(void *parameter);
// Task1: acende o LED amarelo (vermelho + verde) após 10s (tempo que o vermelho e verde são acionados individualmente), e mantém por 3s
void task1(void *parameter);

#endif