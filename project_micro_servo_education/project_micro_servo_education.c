

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// --- Configuração do Pino de Controle do Servo ---
// O pino de sinal do servos estão conectado nos:
#define SERVO_PIN_1 28
#define SERVO_PIN_2 0 // SDA
#define SERVO_PIN_3 2 // SDA

#define PWM_WRAP 20000    // 20ms período
#define PWM_CLKDIV 125.0f // 1us por tick

int servo_pins[] = {SERVO_PIN_1, SERVO_PIN_2, SERVO_PIN_3};
int servo_slice1[3];
int servo_last_value[3] = {90, 90, 90};

/**
 * @brief Converte um ângulo (0-180 graus) para o valor de duty cycle do PWM.
 * @param angle O ângulo desejado para o servo.
 * @return O valor do duty cycle (0-65535) para a função pwm_set_gpio_level.
 */
uint16_t angle_to_duty(float angle)
{
    // A maioria dos servos opera com um pulso entre ~0.5ms e ~2.5ms.
    // Mapeamos o ângulo de 0-180 graus para essa faixa de pulso.
    float pulse_ms = 0.5f + (angle / 180.0f) * 2.0f;

    // Converte a largura do pulso em milissegundos para um valor de duty cycle.
    // O período total do nosso sinal de 50Hz é 20ms.
    // A fórmula é: (largura_do_pulso_ms / período_total_ms)
    // Para um contador de 16 bits, o valor máximo é 65535.
    // No nosso caso, usamos um wrap de 20000 para facilitar, então o cálculo é:
    // Duty = (pulse_ms / 20ms) * 20000
    return (uint16_t)((pulse_ms / 20.0f) * 20000.0f);
}

uint setup_pwm_micro(int servo_pin)
{
    // --- Configuração do Periférico PWM ---

    // 1. Define a função do pino escolhido como PWM.
    gpio_set_function(servo_pin, GPIO_FUNC_PWM);

    // 2. Descobre qual "fatia" (slice) de hardware PWM controla este pino.
    uint slice_num = pwm_gpio_to_slice_num(servo_pin);

    // 3. Configura os parâmetros do PWM para gerar um sinal de 50Hz.
    pwm_config config = pwm_get_default_config();
    // O clock do sistema é 125MHz. Dividindo por 125, temos um "tick" a cada 1us.
    pwm_config_set_clkdiv(&config, PWM_CLKDIV);
    // Definimos o período para 20.000 ticks de 1us = 20.000us = 20ms (que é 50Hz).
    pwm_config_set_wrap(&config, PWM_WRAP);

    // 4. Inicia a fatia de PWM com a configuração definida.
    pwm_init(slice_num, &config, true);

    return slice_num;
}

void servo_angle(int servo_num, int angle)
{
    if (angle > 180 || angle < 0) return;

    int current_angle = servo_last_value[servo_num];

    if (angle == current_angle) return; // já está no ângulo desejado

    // Define direção do movimento
    int step = (angle > current_angle) ? 5 : -5;

    for (int a = current_angle; a != angle; a += step)
    {
        printf("Movendo para o angulo: %d\n", a);
        pwm_set_gpio_level(servo_pins[servo_num], angle_to_duty(a));
        sleep_ms(50);
    }
    // Garante o valor final exato
    pwm_set_gpio_level(servo_pins[servo_num], angle_to_duty(angle));

    servo_last_value[servo_num] = angle; // atualiza o último valor
}

void servos_controllers(int servo_angle_1, int servo_angle_2, int servo_angle_3)
{

    servo_angle(1, servo_angle_1);
    sleep_ms(1000);
    servo_angle(2, servo_angle_2);
    sleep_ms(1000);
    servo_angle(2, servo_angle_3);
    sleep_ms(1000);
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        sleep_ms(100);
    }

    printf("--- Configurando Micros Servos na gpio 0, 2 e 28, usando pwm---\n");

    for (int i = 0; i < 3; i++)
    {
        servo_slice1[i] = setup_pwm_micro(servo_pins[i]);
    }

    printf("PWM inicializado. Movendo o servo...\n");

    // Loop infinito para mover o servo.
    while (1)
    {

        servos_controllers(95, 95, 95);

        sleep_ms(1000); // Pausa no final do curso.

        servos_controllers(85, 85, 85);
    }

    return 0;
}