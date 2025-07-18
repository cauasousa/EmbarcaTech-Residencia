#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <stdint.h>

// Definição dos pinos usados para os servos
#define SERVO_PIN_1 28
#define SERVO_PIN_2 2
#define SERVO_PIN_3 0

// Variáveis globais para os ângulos dos servos
// Declaradas extern para serem acessadas em outros módulos (ex: web_server)
extern int x, y, z;
extern int servo_last_value[3];  // Último valor de pulso enviado para cada servo
extern int servo_pins[3];        // Array com os pinos dos servos

// Inicializa PWM para o pino do servo especificado
void setup_pwm_micro(int servo_pin);

// Define o ângulo do servo (servo_num: 0,1,2 para x,y,z; angle em graus)
void servo_angle(int servo_num, int angle);

// Converte ângulo (float) para valor de duty cycle (uint16_t)
uint16_t angle_to_duty(float angle);

#endif // SERVO_CONTROL_H