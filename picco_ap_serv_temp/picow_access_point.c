/**
 * Projeto: Servidor HTTP com controle de LED além de visualizar a temperatura via Access Point - Raspberry Pi Pico W
 *
 * Objetivos:
 * - Configurar o Raspberry Pi Pico W como um ponto de acesso (Access Point) Wi-Fi.
 * - Iniciar servidores DHCP e DNS locais para permitir a conexão de dispositivos clientes.
 * - Criar um servidor HTTP embarcado que disponibiliza uma página HTML de controle.
 * - Permitir o controle remoto de um LED conectado ao GPIO 0 através de comandos HTTP.
 * - Permitir a visualização da temperatura
 *
 * Funcionalidades:
 * - Criação de uma rede Wi-Fi com nome (SSID) e senha definidos no código.
 * - Atribuição automática de IP aos dispositivos conectados via servidor DHCP.
 * - Interface HTML que permite visualizar e alterar o estado do LED (ligado/desligado).
 * - Manipulação direta de pinos GPIO por meio de requisições do navegador.
 * - Finalização controlada do modo Access Point via tecla 'd'.
 * - Visualizar a temperatura, podendo apertar no botão para atualizar a temperatura
 */

#include <string.h>

#include "pico/cyw43_arch.h"

#include "helpers/temperature/temperature_adc.h"
#include "helpers/tcp/tcp_server.h"
#include "helpers/led/led.h"

#include "hardware/adc.h" // Controle do ADC interno

volatile float current_temp = 0.0f; // Temperatura atual

void key_pressed_func(void *param)
{
    assert(param);
    TCP_SERVER_T *state = (TCP_SERVER_T *)param;
    int key = getchar_timeout_us(0); // get any pending key press but don't wait
    if (key == 'd' || key == 'D')
    {
        cyw43_arch_lwip_begin();
        cyw43_arch_disable_ap_mode();
        cyw43_arch_lwip_end();
        state->complete = true;
    }
}

int main()
{
    stdio_init_all();

    setup_temperature();
    sleep_ms(100);

    led_setup();
    sleep_ms(100);

    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state)
    {
        DEBUG_printf("failed to allocate state\n");
        return 1;
    }

    if (cyw43_arch_init())
    {
        DEBUG_printf("failed to initialise\n");
        return 1;
    }

    // Get notified if the user presses a key
    stdio_set_chars_available_callback(key_pressed_func, state);

    const char *ap_name = "picow_test";
#if 1
    const char *password = "password";
#else
    const char *password = NULL;
#endif

    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

#if LWIP_IPV6
#define IP(x) ((x).u_addr.ip4)
#else
#define IP(x) (x)
#endif

    ip4_addr_t mask;
    IP(state->gw).addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    IP(mask).addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

#undef IP

    // Start the dhcp server
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &state->gw, &mask);

    // Start the dns server
    dns_server_t dns_server;
    dns_server_init(&dns_server, &state->gw);

    if (!tcp_server_open(state, ap_name))
    {
        DEBUG_printf("failed to open server\n");
        return 1;
    }

    state->complete = false;
    while (!state->complete)
    {

        // the following #ifdef is only here so this same example can be used in multiple modes;
        // you do not need it in your code
#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
        // main loop (not from a timer interrupt) to check for Wi-Fi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
        // you can poll as often as you like, however if you have nothing else to do you can
        // choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
#else
        // if you are not using pico_cyw43_arch_poll, then Wi-FI driver and lwIP work
        // is done via interrupt in the background. This sleep is just an example of some (blocking)
        // work you might be doing.
        sleep_ms(1000);
#endif

        // leitura de temperatura
        read_temperature();
    }

    tcp_server_close(state);

    dns_server_deinit(&dns_server);

    dhcp_server_deinit(&dhcp_server);

    cyw43_arch_deinit();

    DEBUG_printf("Test complete\n");

    return 0;
}
