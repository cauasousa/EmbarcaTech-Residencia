#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "src/servo_control/servo_control.h" // Controle dos servos
#include "src/web_server/web_server.h"       // Servidor web
#include "src/wifi_manager/wifi_manager.h"   // Gerenciamento do Wi-Fi

// Estado global do servidor TCP
static TCP_SERVER_T *global_server_state;

// Função que roda no core 1 para controlar os servos
void core1_entry() {
    while (true) {
        uint32_t acao = multicore_fifo_pop_blocking(); // Espera ação via FIFO
        printf("\nDados da FIFO recebido!\n");

        if (acao == ACAO_MOVE_SERVO_X) {
            printf("\nACAO MOVER SERVO NO EIXO X\n");
            servo_angle(0, x);
            sleep_ms(50);
        } else if (acao == ACAO_MOVE_SERVO_Y) {
            printf("\nACAO MOVER SERVO NO EIXO Y\n");
            servo_angle(1, y);
            sleep_ms(50);
        } else if (acao == ACAO_MOVE_SERVO_Z) {
            printf("\nACAO MOVER SERVO NO EIXO Z\n");
            servo_angle(2, z);
            sleep_ms(50);
        }
    }
}

// Função principal (core 0)
int main() {
    stdio_init_all(); // Inicializa entrada/saída padrão
    sleep_ms(100);

    printf("--- Configurando Micros Servos na gpio 0, 2 e 28, usando pwm---\n");
    for (int i = 0; i < 3; i++) {
        setup_pwm_micro(servo_pins[i]); // Configura PWM nos pinos dos servos
    }
    printf("PWM inicializado. Movendo o servo...\n");

    multicore_launch_core1(core1_entry); // Inicia core 1 com a função de controle

    global_server_state = calloc(1, sizeof(TCP_SERVER_T)); // Aloca estado do servidor
    if (!global_server_state) {
        printf("failed to allocate server state\n");
        return 1;
    }

    const char *ap_name = "Braco_robotico";
    const char *password = "123456789";

    wifi_manager_init_ap(global_server_state, ap_name, password); // Inicia AP Wi-Fi e servidor web

    free(global_server_state); // Libera memória após finalização

    printf("Aplicação Finalizada.\n");
    return 0;
}
