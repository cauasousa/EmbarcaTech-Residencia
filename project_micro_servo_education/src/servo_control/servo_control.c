#include "servo_control.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdio.h> // Para printf

// Variáveis globais definidas aqui
int x = 90, y = 90, z = 90;
int servo_pins[] = {SERVO_PIN_1, SERVO_PIN_2, SERVO_PIN_3};
int servo_last_value[3] = {90, 90, 90};

/**
 * @brief Converte um ângulo (0-180 graus) para valor do duty cycle do PWM.
 * @param angle O ângulo desejado para o servo.
 * @return Valor do duty cycle (0-20000) para pwm_set_gpio_level.
 */
uint16_t angle_to_duty(float angle)
{
    // Pulso típico para servo: 0.5ms (0°) até 2.5ms (180°)
    float pulse_ms = 0.5f + (angle / 180.0f) * 2.0f;

    // Período do PWM: 20ms (50Hz), wrap = 20000 ticks (1 tick = 1us)
    // Duty cycle = (pulse_ms / 20ms) * wrap (20000)
    return (uint16_t)((pulse_ms / 20.0f) * 20000.0f);
}

void setup_pwm_micro(int servo_pin)
{
    // Configura o pino para função PWM
    gpio_set_function(servo_pin, GPIO_FUNC_PWM);

    // Obtém o slice PWM responsável pelo pino
    uint slice_num = pwm_gpio_to_slice_num(servo_pin);

    // Configura PWM para 50Hz (periodo 20ms)
    pwm_config config = pwm_get_default_config();

    // Divide o clock (125MHz) para obter 1 tick = 1us
    pwm_config_set_clkdiv(&config, 125.0f);

    // Define o wrap para 20000 ticks (20ms)
    pwm_config_set_wrap(&config, 20000);

    // Inicializa o PWM e ativa
    pwm_init(slice_num, &config, true);
}

void servo_angle(int servo_num, int angle)
{
    if (angle < 0 || angle > 180)
        return;

    int current_angle = servo_last_value[servo_num];
    int step = (angle > current_angle) ? 1 : -1;

    // Move o servo suavemente, ajustando um grau por vez
    for (int a = current_angle; a != angle; a += step)
    {
        printf("Movendo para o angulo: %d\n", a);
        pwm_set_gpio_level(servo_pins[servo_num], angle_to_duty(a));
        sleep_ms(50);
    }

    // Ajusta para o ângulo final exatamente
    pwm_set_gpio_level(servo_pins[servo_num], angle_to_duty(angle));
    printf("Movendo para o angulo: %d\n", angle);

    // Atualiza o último valor conhecido
    servo_last_value[servo_num] = angle;
}
