/**
 * ------------------------------------------------------------
 *  Arquivo: main.c
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Ciclo principal do sistema embarcado, baseado em um
 *      executor cíclico com 3 tarefas principais:
 *
 *      Tarefa 1 - Leitura da temperatura via DMA (meio segundo)
 *      Tarefa 2 - Exibição da temperatura e tendência no OLED
 *      Tarefa 3 - Análise da tendência da temperatura
 *
 *      O sistema utiliza watchdog para segurança, terminal USB
 *      para monitoramento e display OLED para visualização direta.
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include "setup.h"
#include "tarefa1_temp.h"
#include "tarefa2_display.h"
#include "tarefa3_tendencia.h"
#include "tarefa4_controla_neopixel.h"
#include "neopixel_driver.h"
#include "testes_cores.h"
#include "pico/stdio_usb.h"

int64_t tarefa_3(alarm_id_t id, void *user_data);
int64_t tarefa_5(alarm_id_t id, void *user_data);
int64_t tarefa_4(alarm_id_t id, void *user_data);
int64_t tarefa_2(alarm_id_t id, void *user_data);
bool tarefa_1(struct repeating_timer *t);

volatile float media;
tendencia_t t;
volatile absolute_time_t ini_tarefa1, fim_tarefa1, ini_tarefa2, fim_tarefa2, ini_tarefa3, fim_tarefa3, ini_tarefa4, fim_tarefa4;

int main()
{
        setup(); // Inicializações: ADC, DMA, interrupções, OLED, etc.

        struct repeating_timer timer;
        add_repeating_timer_ms(1000, tarefa_1, NULL, &timer); //executar tarefa a cada 1s

        while (true)
        {
                printf("\nTarefas Executando!");
        }

        return 0;
}

/*******************************/
bool tarefa_1(struct repeating_timer *t)
{
        // --- Tarefa 1: Leitura de temperatura via DMA ---
        ini_tarefa1 = get_absolute_time();
        media = tarefa1_obter_media_temp(&cfg_temp, DMA_TEMP_CHANNEL);
        fim_tarefa1 = get_absolute_time();

        add_alarm_in_ms(1000, tarefa_2, NULL, false); // executar tarefa 2 depois de 1s
        return true;
}
/*******************************/
int64_t tarefa_2(alarm_id_t id, void *user_data)
{
        // --- Tarefa 3: Análise da tendência térmica ---
        ini_tarefa3 = get_absolute_time();
        t = tarefa3_analisa_tendencia(media);
        fim_tarefa3 = get_absolute_time();

        add_alarm_in_ms(1000, tarefa_3, NULL, false); // Executar tarefa 3 depois de 1s
        return false;
}
/*******************************/
int64_t tarefa_3(alarm_id_t id, void *user_data)
{
        // --- Tarefa 2: Exibição no OLED ---
        ini_tarefa2 = get_absolute_time();
        tarefa2_exibir_oled(media, t);
        fim_tarefa2 = get_absolute_time();

        add_alarm_in_ms(1000, tarefa_4, NULL, false); // executar tarefa 4 depois de 1s
        return false;
}
/*******************************/
int64_t tarefa_4(alarm_id_t id, void *user_data)
{
        // --- Tarefa 4: Cor da matriz NeoPixel por tendência ---
        absolute_time_t ini_tarefa4 = get_absolute_time();
        tarefa4_matriz_cor_por_tendencia(t);
        absolute_time_t fim_tarefa4 = get_absolute_time();

        add_alarm_in_ms(1000, tarefa_5, NULL, false); // executar tarefa 5 depois de 1s
        return false;
}

int64_t tarefa_5(alarm_id_t id, void *user_data)
{
        // --- Tarefa 5: Extra ---
        while (media < 1)
        {
                npSetAll(COR_BRANCA);
                npWrite();
                sleep_ms(1000);
                npClear();
                npWrite();
                sleep_ms(1000);
        }

        return false;
}