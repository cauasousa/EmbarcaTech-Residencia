#include "wifi_manager.h"
#include "pico/stdlib.h"
#include "dhcpserver.h"
#include "dnsserver.h"
#include <stdio.h> // Para printf

// Função de callback chamada quando uma tecla é pressionada
void key_pressed_func(void *param)
{
    assert(param); // Garante que param não é nulo
    TCP_SERVER_T *state = (TCP_SERVER_T *)param;

    int key = getchar_timeout_us(0); // Captura tecla pressionada (não bloqueante)
    if (key == 'd' || key == 'D') // Se for 'd' ou 'D', desativa AP
    {
        cyw43_arch_lwip_begin();          // Inicia seção crítica da pilha lwIP
        cyw43_arch_disable_ap_mode();     // Desativa modo Access Point
        cyw43_arch_lwip_end();            // Encerra seção crítica
        state->complete = true;           // Marca que o processo foi concluído
    }
}

// Inicializa o modo Access Point, servidor DHCP/DNS e servidor TCP
void wifi_manager_init_ap(TCP_SERVER_T *state, const char *ap_name, const char *password)
{
    if (cyw43_arch_init()) // Inicializa o chip Wi-Fi
    {
        DEBUG_printf("failed to initialise cyw43_arch\n");
        return;
    }

    // Define callback para detectar tecla pressionada
    stdio_set_chars_available_callback(key_pressed_func, state);

    // Ativa modo Access Point com nome e senha fornecidos
    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

    // Configura IP e máscara padrão do AP
    ip4_addr_t mask;
    state->gw.addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    mask.addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

    // Inicializa servidor DHCP
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &state->gw, &mask);

    // Inicializa servidor DNS
    dns_server_t dns_server;
    dns_server_init(&dns_server, &state->gw);

    // Abre servidor TCP
    if (!tcp_server_open(state, ap_name))
    {
        DEBUG_printf("failed to open tcp server\n");
        dns_server_deinit(&dns_server);
        dhcp_server_deinit(&dhcp_server);
        cyw43_arch_deinit();
        return;
    }

    // Aguarda até que o estado seja marcado como completo (ex: tecla 'd' pressionada)
    state->complete = false;
    while (!state->complete)
    {
        sleep_ms(1000); // Aguarda passivamente
    }

    // Finaliza e limpa todos os recursos utilizados
    tcp_server_close(state);
    dns_server_deinit(&dns_server);
    dhcp_server_deinit(&dhcp_server);
    cyw43_arch_deinit();

    printf("WiFi AP and server deinitialized.\n");
}
