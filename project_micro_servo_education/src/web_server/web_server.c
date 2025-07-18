#include "web_server.h"
#include "servo_control/servo_control.h" // Para acessar variáveis e funções dos servos
#include "pico/multicore.h"
#include <string.h>
#include <stdlib.h> // calloc, free
#include <stdio.h>  // snprintf, printf

// Fecha conexão TCP com cliente e limpa estado
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
            tcp_abort(client_pcb); // Força desconexão se fechar falhar
            close_err = ERR_ABRT;
        }
        if (con_state)
        {
            free(con_state); // Libera memória do estado da conexão
        }
    }
    return close_err;
}

// Fecha o servidor TCP principal
void tcp_server_close(TCP_SERVER_T *state)
{
    if (state->server_pcb)
    {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
}

// Callback chamado após envio de dados ao cliente
static err_t tcp_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    con_state->sent_len += len;
    if (con_state->sent_len >= con_state->header_len + con_state->result_len)
    {
        // Após envio completo, zera buffers e encerra conexão
        memset(con_state->headers, 0, sizeof(con_state->headers));
        memset(con_state->result, 0, sizeof(con_state->result));
        return tcp_close_client_connection(con_state, pcb, ERR_OK);
    }
    return ERR_OK;
}

// Gera conteúdo HTML e trata comandos para controle dos servos
static int test_server_content(const char *request, const char *params, char *result, size_t max_result_len)
{
    int len = 0;

    if (strncmp(request, INPUT_TEST, sizeof(INPUT_TEST) - 1) == 0)
    {
        if (params)
        {
            char *p;
            int changed = -1;
            int step_value = 0; // valor do passo para ajuste do ângulo

            DEBUG_printf("Requisão Feita!\n");

            // Verifica parâmetro para ajuste de ângulo nos eixos x, y, z
            if ((p = strstr(params, "x_plus1=")) != NULL)      { step_value = 1;  changed = 0; }
            else if ((p = strstr(params, "x_minus1=")) != NULL) { step_value = -1; changed = 0; }
            else if ((p = strstr(params, "x_plus10=")) != NULL) { step_value = 20; changed = 0; }
            else if ((p = strstr(params, "x_minus10=")) != NULL){ step_value = -20;changed = 0; }
            else if ((p = strstr(params, "y_plus1=")) != NULL)  { step_value = 1;  changed = 1; }
            else if ((p = strstr(params, "y_minus1=")) != NULL) { step_value = -1; changed = 1; }
            else if ((p = strstr(params, "y_plus10=")) != NULL) { step_value = 20; changed = 1; }
            else if ((p = strstr(params, "y_minus10=")) != NULL){ step_value = -20;changed = 1; }
            else if ((p = strstr(params, "z_plus1=")) != NULL)  { step_value = 1;  changed = 2; }
            else if ((p = strstr(params, "z_minus1=")) != NULL) { step_value = -1; changed = 2; }
            else if ((p = strstr(params, "z_plus10=")) != NULL) { step_value = 20; changed = 2; }
            else if ((p = strstr(params, "z_minus10=")) != NULL){ step_value = -20;changed = 2; }

            // Aplica limite nos ângulos e notifica núcleo secundário
            if (changed == 0)
            {
                x += step_value;
                if (x < 0) x = 0;
                if (x > 180) x = 180;
                DEBUG_printf("Dado Enviado ao Nucleo 1 (ACAO MOVER SERVO NO EIXO X)\n");
                multicore_fifo_push_blocking(ACAO_MOVE_SERVO_X);
            }
            else if (changed == 1)
            {
                y += step_value;
                if (y < 0) y = 0;
                if (y > 180) y = 180;
                DEBUG_printf("Dado Enviado ao Nucleo 1 (ACAO MOVER SERVO NO EIXO Y)\n");
                multicore_fifo_push_blocking(ACAO_MOVE_SERVO_Y);
            }
            else if (changed == 2)
            {
                z += step_value;
                if (z < 0) z = 0;
                if (z > 180) z = 180;
                DEBUG_printf("Dado Enviado ao Nucleo 1 (ACAO MOVER SERVO NO EIXO Z)\n");
                multicore_fifo_push_blocking(ACAO_MOVE_SERVO_Z);
            }
        }
    }

    // Gera página HTML com os controles e ângulos atuais
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
                   ".btn-small { font-size: 16px; width: 40px; height: 40px; margin: 3px; }"
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

    return len;
}

// Recebe dados HTTP do cliente e responde adequadamente
err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    if (!p)
    {
        DEBUG_printf("Conexao encerrada!\n");
        return tcp_close_client_connection(con_state, pcb, ERR_OK);
    }
    assert(con_state && con_state->pcb == pcb);

    if (p->tot_len > 0)
    {
        // Copia a requisição para o buffer
        pbuf_copy_partial(p, con_state->headers,
                          p->tot_len > sizeof(con_state->headers) - 1 ? sizeof(con_state->headers) - 1 : p->tot_len, 0);

        // Verifica se é requisição GET
        if (strncmp(HTTP_GET, con_state->headers, sizeof(HTTP_GET) - 1) == 0)
        {
            char *request = con_state->headers + sizeof(HTTP_GET); // + espaço
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

            // Gera conteúdo da página
            con_state->result_len = test_server_content(request, params, con_state->result, sizeof(con_state->result));
            DEBUG_printf("Requisição Feita!\n");

            // Verifica tamanho do resultado para evitar overflow
            if (con_state->result_len > sizeof(con_state->result) - 1)
            {
                return tcp_close_client_connection(con_state, pcb, ERR_CLSD);
            }

            // Prepara cabeçalho HTTP para envio
            if (con_state->result_len > 0)
            {
                con_state->header_len = snprintf(con_state->headers, sizeof(con_state->headers), HTTP_RESPONSE_HEADERS,
                                                 200, con_state->result_len);
                if (con_state->header_len > sizeof(con_state->headers) - 1)
                {
                    return tcp_close_client_connection(con_state, pcb, ERR_CLSD);
                }
            }
            else
            {
                // Redireciona se não houver conteúdo
                con_state->header_len = snprintf(con_state->headers, sizeof(con_state->headers), HTTP_RESPONSE_REDIRECT,
                                                 ipaddr_ntoa(con_state->gw));
            }

            // Envia cabeçalho
            con_state->sent_len = 0;
            err_t err = tcp_write(pcb, con_state->headers, con_state->header_len, 0);
            if (err != ERR_OK)
            {
                return tcp_close_client_connection(con_state, pcb, err);
            }

            // Envia corpo da resposta
            if (con_state->result_len)
            {
                err = tcp_write(pcb, con_state->result, con_state->result_len, 0);
                if (err != ERR_OK)
                {
                    return tcp_close_client_connection(con_state, pcb, err);
                }
            }
        }
        tcp_recved(pcb, p->tot_len);
    }
    pbuf_free(p);
    return ERR_OK;
}

// Chamada periódica caso cliente não envie dados
static err_t tcp_server_poll(void *arg, struct tcp_pcb *pcb)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    return tcp_close_client_connection(con_state, pcb, ERR_OK); // desconecta cliente inativo
}

// Tratamento de erro na conexão TCP
static void tcp_server_err(void *arg, err_t err)
{
    TCP_CONNECT_STATE_T *con_state = (TCP_CONNECT_STATE_T *)arg;
    if (err != ERR_ABRT)
    {
        tcp_close_client_connection(con_state, con_state->pcb, err);
    }
}

// Aceita nova conexão TCP e configura callbacks
static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    if (err != ERR_OK || client_pcb == NULL)
    {
        DEBUG_printf("Falha em aceitar\n");
        return ERR_VAL;
    }
    DEBUG_printf("Usuario conectado\n");

    // Aloca estrutura de estado para nova conexão
    TCP_CONNECT_STATE_T *con_state = calloc(1, sizeof(TCP_CONNECT_STATE_T));
    if (!con_state)
    {
        DEBUG_printf("Falha para Alocar Estado\n");
        return ERR_MEM;
    }
    con_state->pcb = client_pcb;
    con_state->gw = &state->gw;

    // Configura callbacks da conexão
    tcp_arg(client_pcb, con_state);
    tcp_sent(client_pcb, tcp_server_sent);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

// Inicializa servidor TCP na porta definida
bool tcp_server_open(void *arg, const char *ap_name)
{
    TCP_SERVER_T *state = (TCP_SERVER_T *)arg;
    DEBUG_printf("Iniciando o servidor na porta %d\n", TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb)
    {
        DEBUG_printf("Falha em criar o pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, IP_ANY_TYPE, TCP_PORT);
    if (err)
    {
        DEBUG_printf("Falha na porta %d\n", TCP_PORT);
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb)
    {
        DEBUG_printf("Falha em escutar\n");
        if (pcb)
        {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcp_server_accept);

    printf("Tente conectar '%s' (precione 'd' para desabilitar o Access Point)\n", ap_name);
    return true;
}
