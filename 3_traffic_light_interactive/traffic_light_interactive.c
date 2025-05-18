#include <stdio.h>
#include "setup.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"


alarm_id_t alarme_id; // id do alarme de led ativo
alarm_id_t alarme_beeps[6]; // Lista de alarmes programados para controlar os beeps
volatile int qnt_alarme_beep = 0; // Quantidade de beeps programados

bool pedestre_pressionou = false; // Flag para saber se o botão de pedestre foi pressionado
volatile int contador = 0;  // Contador para exibir a contagem regressiva

// Declaração das funções utilizadas
void acionar_leds(bool led_verde, bool led_vermelho);
void gpio_callback(uint gpio, uint32_t events);
int64_t ligar_led_vermelho_callback(alarm_id_t id, void *user_data);
int64_t ligar_led_verde_callback(alarm_id_t id, void *user_data);
int64_t ligar_led_amarelo_callback(alarm_id_t id, void *user_data);
bool contador_serial(repeating_timer_t *t);

void desativar_beeps_antigos()
{
    // Cancela todos os beeps anteriores
    for (int i = 0; i < qnt_alarme_beep; i++)
    {
        cancel_alarm(alarme_beeps[i]);
    }
}
void beep_vermelho()
{
    // Define os alarmes para beeps no sinal vermelho (três beeps)
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
    // Beeps no sinal verde (dois beeps)
    desativar_beeps_antigos();
    alarme_beeps[0] = add_alarm_in_ms(100, beep, NULL, false);
    alarme_beeps[1] = add_alarm_in_ms(600, beep_desativar, NULL, false);
    alarme_beeps[2] = add_alarm_in_ms(1200, beep, NULL, false);
    alarme_beeps[3] = add_alarm_in_ms(1700, beep_desativar, NULL, false);
    qnt_alarme_beep = 4;
}

void beep_amarelo()
{
    // Beep curto no sinal amarelo (um bip)
    desativar_beeps_antigos();
    alarme_beeps[0] = add_alarm_in_ms(100, beep, NULL, false);
    alarme_beeps[1] = add_alarm_in_ms(600, beep_desativar, NULL, false);
    qnt_alarme_beep = 2;
}

void acionar_leds(bool led_verde, bool led_vermelho)
{
    // Ativa ou desativa os LEDs conforme parâmetros
    gpio_put(LED_VERMELHO, led_vermelho);
    gpio_put(LED_VERDE, led_verde);
}

void pedestre_pressionou_funcao()
{
    // Quando botão de pedestre é pressionado
    pedestre_pressionou = true;

    cancel_alarm(alarme_id); // Cancela o alarme atual de leds

    acionar_leds(true, true); // liga o amarelo

    beep_amarelo(); // Beep

    alarme_id = add_alarm_in_ms(3000, ligar_led_vermelho_callback, NULL, false); // Após 3s, muda para vermelho
}

void gpio_callback(uint gpio, uint32_t events)
{
    // Função de callback quando algum botão é pressionado
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
    // Ativa o sinal amarelo e define próximo estado como vermelho
    acionar_leds(true, true);
    alarme_id = add_alarm_in_ms(3000, ligar_led_vermelho_callback, NULL, false);
    beep_amarelo();
    printf("\nSinal: Amarelo");

    return 0;
}

int64_t ligar_led_verde_callback(alarm_id_t id, void *user_data)
{
    // Ativa o sinal verde e programa troca para amarelo
    acionar_leds(true, false);
    pedestre_pressionou = false;
    alarme_id = add_alarm_in_ms(10000, ligar_led_amarelo_callback, NULL, false);
    beep_verde();
    printf("\nSinal: Verde");

    return 0;
}

int64_t ligar_led_vermelho_callback(alarm_id_t id, void *user_data)
{
    // Ativa o sinal vermelho e programa troca para verde
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
    // Mostra a contagem para o pedestre
    if (contador <= 0)
    {
        return false; // Para o timer
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
    setup(); // Inicializa GPIOs e periféricos

    alarme_id = add_alarm_in_ms(1, ligar_led_vermelho_callback, NULL, false); // Começa com sinal vermelho

    while (true)
    {
        tight_loop_contents();
    }
}
