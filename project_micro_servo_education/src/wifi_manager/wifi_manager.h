#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// Evita inclusão múltipla do cabeçalho

#include <stdbool.h>
#include "lwip/ip4_addr.h"        // Manipulação de endereços IP (IPv4)
#include "pico/cyw43_arch.h"      // Suporte ao chip Wi-Fi CYW43 do Raspberry Pi Pico W
#include "web_server/web_server.h" // Estrutura TCP_SERVER_T usada no servidor web

// Inicializa o modo Access Point (AP) e inicia o servidor web
void wifi_manager_init_ap(TCP_SERVER_T *state, const char *ap_name, const char *password);

// Função de callback para detectar quando uma tecla (botão) é pressionada
void key_pressed_func(void *param);

#endif // WIFI_MANAGER_H
