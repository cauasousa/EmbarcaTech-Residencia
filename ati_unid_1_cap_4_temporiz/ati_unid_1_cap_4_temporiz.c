#include <stdio.h>
#include "setup.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

alarm_id_t alarme_id;
alarm_id_t alarme_beeps[6];
volatile int qnt_alarme_beep = 0;

bool pedestre_pressionou = false;
volatile int contador = 0;

void acionar_leds(bool led_verde, bool led_vermelho);
void gpio_callback(uint gpio, uint32_t events);
int64_t ligar_led_vermelho_callback(alarm_id_t id, void *user_data);
int64_t ligar_led_verde_callback(alarm_id_t id, void *user_data);
int64_t ligar_led_amarelo_callback(alarm_id_t id, void *user_data);
bool contador_serial(repeating_timer_t *t);

void desativar_beeps_antigos()
{
    for (int i = 0; i < qnt_alarme_beep; i++)
    {
        cancel_alarm(alarme_beeps[i]);
    }
}
void beep_vermelho()
{
    desativar_beeps_antigos();
    alarme_beeps[0] = add_alarm_in_ms(100, beep, NULL, false);
    alarme_beeps[1] = add_alarm_in_ms(600, beep_desativar, NULL, false);
    alarme_beeps[2] = add_alarm_in_ms(1200, beep, NULL, false);
    alarme_beeps[3] = add_alarm_in_ms(1700, beep_desativar, NULL, false);
    alarme_beeps[4] = add_alarm_in_ms(2200, beep, NULL, false);
    alarme_beeps[5] = add_alarm_in_ms(2700, beep_desativar, NULL, false);
    qnt_alarme_beep = 6;
}

void beep_verde()
{
    desativar_beeps_antigos();

    alarme_beeps[0] = add_alarm_in_ms(100, beep, NULL, false);
    alarme_beeps[1] = add_alarm_in_ms(600, beep_desativar, NULL, false);
    alarme_beeps[2] = add_alarm_in_ms(1200, beep, NULL, false);
    alarme_beeps[3] = add_alarm_in_ms(1700, beep_desativar, NULL, false);
    qnt_alarme_beep = 4;
}

void beep_amarelo()
{
    desativar_beeps_antigos();

    alarme_beeps[0] = add_alarm_in_ms(100, beep, NULL, false);
    alarme_beeps[1] = add_alarm_in_ms(600, beep_desativar, NULL, false);
    qnt_alarme_beep = 2;
}

void acionar_leds(bool led_verde, bool led_vermelho)
{
    gpio_put(LED_VERMELHO, led_vermelho);
    gpio_put(LED_VERDE, led_verde);
}

void pedestre_pressionou_funcao()
{

    pedestre_pressionou = true;

    cancel_alarm(alarme_id);

    acionar_leds(true, true);

    beep_amarelo();

    alarme_id = add_alarm_in_ms(3000, ligar_led_vermelho_callback, NULL, false);
}

void gpio_callback(uint gpio, uint32_t events)
{
    if ((events & GPIO_IRQ_EDGE_FALL))
    {
        // Botão A tem prioridade sobre o Botão B
        if (gpio == BOTAO_A)
        {
            if (!pedestre_pressionou)
            {
                pedestre_pressionou_funcao();
                printf("\nBotão A de Pedestres acionado!");
            }
        }
        else if (gpio == BOTAO_B)
        {
            if (!pedestre_pressionou)
            {
                pedestre_pressionou_funcao();
                printf("\nBotão B de Pedestres acionado!");
            }
        }
    }
}

int64_t ligar_led_amarelo_callback(alarm_id_t id, void *user_data)
{
    acionar_leds(true, true);
    alarme_id = add_alarm_in_ms(3000, ligar_led_vermelho_callback, NULL, false);

    beep_amarelo();

    printf("\nSinal: Amarelo");

    return 0;
}

int64_t ligar_led_verde_callback(alarm_id_t id, void *user_data)
{
    acionar_leds(true, false);

    pedestre_pressionou = false;

    alarme_id = add_alarm_in_ms(10000, ligar_led_amarelo_callback, NULL, false);

    beep_verde();

    printf("\nSinal: Verde");

    return 0;
}

int64_t ligar_led_vermelho_callback(alarm_id_t id, void *user_data)
{
    static repeating_timer_t timer;

    if (pedestre_pressionou)
    {
        contador = 10;
        add_repeating_timer_ms(950, contador_serial, NULL, &timer); // Mostrar a contagem
    }

    acionar_leds(false, true);
    alarme_id = add_alarm_in_ms(10000, ligar_led_verde_callback, NULL, false);

    beep_vermelho();

    printf("\nSinal: Vermelho");

    return 0;
}

bool contador_serial(repeating_timer_t *t)
{
    if (contador <= 0)
    {
        return false;
    }
    else if (contador > 5)
    {
        contador -= 1;
        return true;
    }

    printf("\nContagem: %d", contador);
    contador -= 1;

    return true;
}

int main()
{
    setup();

    alarme_id = add_alarm_in_ms(1, ligar_led_vermelho_callback, NULL, false);
    // beep(1000, 1);

    while (true)
    {
        tight_loop_contents();
    }
}
