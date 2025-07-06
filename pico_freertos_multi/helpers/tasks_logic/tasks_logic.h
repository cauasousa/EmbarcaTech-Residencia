#ifndef TASKS_LOGIC_H
#define TASKS_LOGIC_H

#include "pico/stdlib.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
// #include "../setup/setup.h"
#include "../leds/leds.h"
#include "../buzzer/buzzer.h"
#include "../botoes/botoes.h"
#include "../joystick/joystick.h"
#include "../micro/micro.h"

void monitoring_task(void *parameter);
void alive_task(void *parameter);
void self_test(void *parameter);

#endif