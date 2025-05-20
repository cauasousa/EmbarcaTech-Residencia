#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "buzzer.h"

// Definição de uma função para emitir um beep com duração especificada
void active_beep()
{
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(BUZZER_PIN, 2048);
}

void desactive_beep()
{
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void touch_buzzer()
{
    active_beep(); // ativa o buzzer
    sleep_ms(1000); // deixa o buzzer ligado por 1s
    desactive_beep(); // desativa buzzer
}

void buzzer_setup()
{
    // Configurar o pino como saída de PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(BUZZER_PIN, 0);
}