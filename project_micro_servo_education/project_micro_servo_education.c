#include <string.h>
#include "pico/multicore.h"
#include "hardware/pwm.h"

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "dhcpserver.h"
#include "dnsserver.h"

#define TCP_PORT 80
#define DEBUG_printf printf
#define POLL_TIME_S 5
#define HTTP_GET "GET"
#define HTTP_RESPONSE_HEADERS "HTTP/1.1 %d OK\nContent-Length: %d\nContent-Type: text/html; charset=utf-8\nConnection: close\n\n"
#define INPUT_TEST "/input"
#define LED_TEST_BODY "<html><body><form action=\"/input\" method=\"get\">x: <input name=\"x\" type=\"text\"><br>y: <input name=\"y\" type=\"text\"><br>z: <input name=\"z\" type=\"text\"><br><input type=\"submit\" value=\"Enviar\"></form></body></html>"
// #define HTTP_RESPONSE_REDIRECT " "
#define HTTP_RESPONSE_REDIRECT "HTTP/1.1 302 Redirect\nLocation: http://%s\n\n"

#define ACAO_MOVE_SERVO_X 0
#define ACAO_MOVE_SERVO_Y 1
#define ACAO_MOVE_SERVO_Z 2
int x = 90, y = 90, z = 90;

// --- Configuração do Pino de Controle do Servo ---
// O pino de sinal do servos estão conectado nos:
#define SERVO_PIN_1 28
#define SERVO_PIN_2 2 // SDA
#define SERVO_PIN_3 0 // SDA

#define PWM_WRAP 20000    // 20ms período
#define PWM_CLKDIV 125.0f // 1us por tick

int servo_pins[] = {SERVO_PIN_1, SERVO_PIN_2, SERVO_PIN_3};
int servo_slice1[3];
int servo_last_value[3] = {90, 90, 90};

typedef struct TCP_SERVER_T_
{
    struct tcp_pcb *server_pcb;
    bool complete;
    ip_addr_t gw;
} TCP_SERVER_T;

typedef struct TCP_CONNECT_STATE_T_
{
    struct tcp_pcb *pcb;
    int sent_len;
    char headers[256];
    char result[2048];
    int header_len;
    int result_len;
    ip_addr_t *gw;
} TCP_CONNECT_STATE_T;

static err_t tcp_close_client_connection(TCP_CONNECT_STATE_T *con_state, struct tcp_pcb *client_pcb, err_t close_err)
{
    if (client_pcb)
    {
        assert(con_state && con_state->pcb == client_pcb);
        tcp_arg(client_pcb, NULL);
        tcp_poll(client_pcb, NULL, 0);
        tcp_sent(client_pcb, NULL);
        tcp_recv(client_pcb, NULL);
        tcp_err(client_pcb, NULL);
        err_t err = tcp_close(client_pcb);
        if (err != ERR_OK)
        {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(client_pcb);
            close_err = ERR_ABRT;
        }
        if (con_state)
        {
            free(con_state);
        }
    }
    return close_err;
}

static void tcp_server_close(TCP_SERVER_T *state)
{
    if (state->server_pcb)
    {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
}

static err_t tcp_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    DEBUG_printf("tcp_server_sent %u\n", len);
    con_state->sent_len += len;
    if (con_state->sent_len >= con_state->header_len + con_state->result_len)
    {
        DEBUG_printf("all done\n");
        // Zera os buffers
        memset(con_state->headers, 0, sizeof(con_state->headers));
        memset(con_state->result, 0, sizeof(con_state->result));
        return tcp_close_client_connection(con_state, pcb, ERR_OK);
    }
    return ERR_OK;
}

static int test_server_content(const char *request, const char *params, char *result, size_t max_result_len)
{
    int len = 0;

    if (strncmp(request, INPUT_TEST, sizeof(INPUT_TEST) - 1) == 0)
    {
        int value_x = 0, value_y = 0, value_z = 0;
        const char *mensagem = ""; // mensagem padrão (vazia)

        if (params)
        {
            char *p;
            int changed = -1;
            int step_value = 0; // Variável para armazenar o valor do passo (+1, -1, +10, -10)

            // Verifica os parâmetros para cada eixo e define o valor do passo
            if ((p = strstr(params, "x_plus1=")) != NULL)
            {
                step_value = 1;
                changed = 0;
            }
            else if ((p = strstr(params, "x_minus1=")) != NULL)
            {
                step_value = -1;
                changed = 0;
            }
            else if ((p = strstr(params, "x_plus10=")) != NULL)
            {
                step_value = 20;
                changed = 0;
            }
            else if ((p = strstr(params, "x_minus10=")) != NULL)
            {
                step_value = -20;
                changed = 0;
            }
            else if ((p = strstr(params, "y_plus1=")) != NULL)
            {
                step_value = 1;
                changed = 1;
            }
            else if ((p = strstr(params, "y_minus1=")) != NULL)
            {
                step_value = -1;
                changed = 1;
            }
            else if ((p = strstr(params, "y_plus10=")) != NULL)
            {
                step_value = 20;
                changed = 1;
            }
            else if ((p = strstr(params, "y_minus10=")) != NULL)
            {
                step_value = -20;
                changed = 1;
            }
            else if ((p = strstr(params, "z_plus1=")) != NULL)
            {
                step_value = 1;
                changed = 2;
            }
            else if ((p = strstr(params, "z_minus1=")) != NULL)
            {
                step_value = -1;
                changed = 2;
            }
            else if ((p = strstr(params, "z_plus10=")) != NULL)
            {
                step_value = 20;
                changed = 2;
            }
            else if ((p = strstr(params, "z_minus10=")) != NULL)
            {
                step_value = -20;
                changed = 2;
            }

            if (changed == 0)
            {
                x += step_value;
                if (x < 0)
                    x = 0;
                if (x > 180)
                    x = 180;
                multicore_fifo_push_blocking(ACAO_MOVE_SERVO_X);
            }
            else if (changed == 1)
            {
                y += step_value;
                if (y < 0)
                    y = 0;
                if (y > 180)
                    y = 180;
                multicore_fifo_push_blocking(ACAO_MOVE_SERVO_Y);
            }
            else if (changed == 2)
            {
                z += step_value;
                if (z < 0)
                    z = 0;
                if (z > 180)
                    z = 180;
                multicore_fifo_push_blocking(ACAO_MOVE_SERVO_Z);
            }
        }
    }

    // Retorna a página com mensagem incluída e os novos botões
    len = snprintf(result, max_result_len,
                   "<!DOCTYPE html><html lang=\"pt-BR\">"
                   "<head><meta charset=\"UTF-8\"><title>Controle Servo</title>"
                   "<style>"
                   "body { background-color: #f0f8ff; font-family: Arial, sans-serif; text-align: center; padding: 40px; }"
                   ".servo-box { margin-bottom: 30px; }"
                   ".angle { font-size: 24px; margin: 10px; }"
                   ".btn { font-size: 20px; width: 50px; height: 50px; margin: 5px; border-radius: 50%%; border: none; color: white; cursor: pointer; }"
                   ".btn:hover { opacity: 0.8; }"
                   ".x { background-color: #f44336; }"
                   ".y { background-color: #4CAF50; }"
                   ".z { background-color: #2196F3; }"
                   ".btn-small { font-size: 16px; width: 40px; height: 40px; margin: 3px; }" // Novo estilo para botões pequenos
                   "</style></head><body>"
                   "<h1>Controle de Servos</h1>"

                   "<div class='servo-box'>"
                   "<h2>Eixo X: %d°</h2>"
                   "<a href='/input?x_minus10=1'><button class='btn x'>-20</button></a>"
                   "<a href='/input?x_minus1=1'><button class='btn x btn-small'>-1</button></a>"
                   "<a href='/input?x_plus1=1'><button class='btn x btn-small'>+1</button></a>"
                   "<a href='/input?x_plus10=1'><button class='btn x'>+20</button></a>"
                   "</div>"

                   "<div class='servo-box'>"
                   "<h2>Eixo Y: %d°</h2>"
                   "<a href='/input?y_minus10=1'><button class='btn y'>-20</button></a>"
                   "<a href='/input?y_minus1=1'><button class='btn y btn-small'>-1</button></a>"
                   "<a href='/input?y_plus1=1'><button class='btn y btn-small'>+1</button></a>"
                   "<a href='/input?y_plus10=1'><button class='btn y'>+20</button></a>"
                   "</div>"

                   "<div class='servo-box'>"
                   "<h2>Eixo Z: %d°</h2>"
                   "<a href='/input?z_minus10=1'><button class='btn z'>-20</button></a>"
                   "<a href='/input?z_minus1=1'><button class='btn z btn-small'>-1</button></a>"
                   "<a href='/input?z_plus1=1'><button class='btn z btn-small'>+1</button></a>"
                   "<a href='/input?z_plus10=1'><button class='btn z'>+20</button></a>"
                   "</div>"

                   "</body></html>",
                   x, y, z);
    // else if (strncmp(request, "/", 1) == 0)
    // {
    //     // página inicial
    //     len = snprintf(result, max_result_len,
    //                    "<!DOCTYPE html><html><head><meta charset=\"UTF-8\">"
    //                    "<title>Controle</title>"
    //                    "<style>body { background-color: #e0f7fa; font-family: sans-serif; text-align: center; padding-top: 50px; }</style>"
    //                    "</head><body>"
    //                    "<h1>Controle do Servo</h1>"
    //                    "<p><a href=\"/input\">Ir para o formulário de controle</a></p>"
    //                    "</body></html>");
    // }

    return len;
}

err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    if (!p)
    {
        DEBUG_printf("connection closed\n");
        return tcp_close_client_connection(con_state, pcb, ERR_OK);
    }
    assert(con_state && con_state->pcb == pcb);
    if (p->tot_len > 0)
    {
        DEBUG_printf("tcp_server_recv %d err %d\n", p->tot_len, err);
#if 0
        for (struct pbuf *q = p; q != NULL; q = q->next) {
            DEBUG_printf("in: %.*s\n", q->len, q->payload);
        }
#endif
        // Copy the request into the buffer
        pbuf_copy_partial(p, con_state->headers, p->tot_len > sizeof(con_state->headers) - 1 ? sizeof(con_state->headers) - 1 : p->tot_len, 0);

        // Handle GET request
        if (strncmp(HTTP_GET, con_state->headers, sizeof(HTTP_GET) - 1) == 0)
        {
            char *request = con_state->headers + sizeof(HTTP_GET); // + space
            char *params = strchr(request, '?');
            if (params)
            {
                if (*params)
                {
                    char *space = strchr(request, ' ');
                    *params++ = 0;
                    if (space)
                    {
                        *space = 0;
                    }
                }
                else
                {
                    params = NULL;
                }
            }

            // Generate content
            con_state->result_len = test_server_content(request, params, con_state->result, sizeof(con_state->result));
            DEBUG_printf("Request: %s?%s\n", request, params);
            DEBUG_printf("Result: %d\n", con_state->result_len);

            // Check we had enough buffer space
            if (con_state->result_len > sizeof(con_state->result) - 1)
            {
                DEBUG_printf("Too much result data %d\n", con_state->result_len);
                return tcp_close_client_connection(con_state, pcb, ERR_CLSD);
            }

            // Generate web page
            if (con_state->result_len > 0)
            {
                con_state->header_len = snprintf(con_state->headers, sizeof(con_state->headers), HTTP_RESPONSE_HEADERS,
                                                 200, con_state->result_len);
                if (con_state->header_len > sizeof(con_state->headers) - 1)
                {
                    DEBUG_printf("Too much header data %d\n", con_state->header_len);
                    return tcp_close_client_connection(con_state, pcb, ERR_CLSD);
                }
            }
            else
            {
                // Send redirect
                con_state->header_len = snprintf(con_state->headers, sizeof(con_state->headers), HTTP_RESPONSE_REDIRECT,
                                                 ipaddr_ntoa(con_state->gw));
                DEBUG_printf("Sending redirect %s", con_state->headers);
            }

            // Send the headers to the client
            con_state->sent_len = 0;
            err_t err = tcp_write(pcb, con_state->headers, con_state->header_len, 0);
            if (err != ERR_OK)
            {
                DEBUG_printf("failed to write header data %d\n", err);
                return tcp_close_client_connection(con_state, pcb, err);
            }

            // Send the body to the client
            if (con_state->result_len)
            {
                err = tcp_write(pcb, con_state->result, con_state->result_len, 0);
                if (err != ERR_OK)
                {
                    DEBUG_printf("failed to write result data %d\n", err);
                    return tcp_close_client_connection(con_state, pcb, err);
                }
            }
        }
        tcp_recved(pcb, p->tot_len);
    }
    pbuf_free(p);
    return ERR_OK;
}

static err_t tcp_server_poll(void *arg, struct tcp_pcb *pcb)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    DEBUG_printf("tcp_server_poll_fn\n");
    return tcp_close_client_connection(con_state, pcb, ERR_OK); // Just disconnect clent?
}

static void tcp_server_err(void *arg, err_t err)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    if (err != ERR_ABRT)
    {
        DEBUG_printf("tcp_client_err_fn %d\n", err);
        tcp_close_client_connection(con_state, con_state->pcb, err);
    }
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    if (err != ERR_OK || client_pcb == NULL)
    {
        DEBUG_printf("failure in accept\n");
        return ERR_VAL;
    }
    DEBUG_printf("client connected\n");

    // Create the state for the connection
    TCP_CONNECT_STATE_T *con_state = calloc(1, sizeof(TCP_CONNECT_STATE_T));
    if (!con_state)
    {
        DEBUG_printf("failed to allocate connect state\n");
        return ERR_MEM;
    }
    con_state->pcb = client_pcb; // for checking
    con_state->gw = &state->gw;

    // setup connection to client
    tcp_arg(client_pcb, con_state);
    tcp_sent(client_pcb, tcp_server_sent);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

static bool tcp_server_open(void *arg, const char *ap_name)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    DEBUG_printf("starting server on port %d\n", TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb)
    {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, IP_ANY_TYPE, TCP_PORT);
    if (err)
    {
        DEBUG_printf("failed to bind to port %d\n", TCP_PORT);
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb)
    {
        DEBUG_printf("failed to listen\n");
        if (pcb)
        {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcp_server_accept);

    printf("Try connecting to '%s' (press 'd' to disable access point)\n", ap_name);
    return true;
}

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
    pwm_config_set_clkdiv(&config, 125.0f);
    // Definimos o período para 20.000 ticks de 1us = 20.000us = 20ms (que é 50Hz).
    pwm_config_set_wrap(&config, 20000);

    // 4. Inicia a fatia de PWM com a configuração definida.
    pwm_init(slice_num, &config, true);

    return slice_num;
}

void servo_angle(int servo_num, int angle)
{
    printf("Caso 1");
    if (angle > 180 || angle < 0)
        return;

    printf("Caso 2");
    int current_angle = servo_last_value[servo_num];

    printf("Caso 3");
    // Define direção do movimento
    int step = (angle > current_angle) ? 1 : -1;

    // printf("Caso 4 -%d - %d - %d", step, current_angle, angle);
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

    printf("\nMove servo no eixo X ...\n");

    servo_angle(0, servo_angle_1);
    sleep_ms(50);
    printf("\nMove servo no eixo y ...\n");

    servo_angle(1, servo_angle_2);
    sleep_ms(50);
    printf("\nMove servo no eixo z ...\n");

    servo_angle(2, servo_angle_3);
    sleep_ms(50);
}

void tratar_eventos_web()
{

    while (true)
    {
        uint32_t acao = multicore_fifo_pop_blocking(); // Espera ação
        if (acao == ACAO_MOVE_SERVO_X)
        {
            servo_angle(0, x);
            sleep_ms(50);
        }
        else if (acao == ACAO_MOVE_SERVO_Y)
        {
            servo_angle(1, y);
            sleep_ms(50);
        }
        else if (acao == ACAO_MOVE_SERVO_Z)
        {
            servo_angle(2, z);
            sleep_ms(50);
        }
    }
}
int main()
{
    stdio_init_all();
    // while (!stdio_usb_connected())
    // {
    //      sleep_ms(100);
    // }
    sleep_ms(100);
    printf("--- Configurando Micros Servos na gpio 0, 2 e 28, usando pwm---\n");

    for (int i = 0; i < 3; i++)
    {
        servo_slice1[i] = setup_pwm_micro(servo_pins[i]);
    }

    printf("PWM inicializado. Movendo o servo...\n");

    multicore_launch_core1(tratar_eventos_web);

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

    stdio_set_chars_available_callback(key_pressed_func, state);

    const char *ap_name = "Braco_robotico";
    const char *password = "123456789";

    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

    ip4_addr_t mask;
    state->gw.addr = PP_HTONL(CYW43_DEFAULT_IP_AP_ADDRESS);
    mask.addr = PP_HTONL(CYW43_DEFAULT_IP_MASK);

    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &state->gw, &mask);

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
        sleep_ms(1000); // No modo background, apenas aguarda
    }

    tcp_server_close(state);
    dns_server_deinit(&dns_server);
    dhcp_server_deinit(&dhcp_server);
    cyw43_arch_deinit();
    printf("Test complete\n");
    return 0;
}