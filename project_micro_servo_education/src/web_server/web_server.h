#ifndef WEB_SERVER_H
#define WEB_SERVER_H

// Evita inclusão múltipla do cabeçalho

#include <stdbool.h>
#include "lwip/tcp.h"      // Suporte a conexões TCP (usando lwIP)
#include "lwip/pbuf.h"     // Buffers de pacotes
#include "lwip/ip4_addr.h" // Endereços IPv4

// Configurações básicas do servidor
#define TCP_PORT 80                               // Porta padrão HTTP
#define DEBUG_printf printf                       // Alias para facilitar debug
#define POLL_TIME_S 5                             // Tempo entre verificações (poll) TCP
#define HTTP_GET "GET"                            // Método HTTP GET
#define HTTP_RESPONSE_HEADERS "HTTP/1.1 %d OK\nContent-Length: %d\nContent-Type: text/html; charset=utf-8\nConnection: close\n\n"
#define INPUT_TEST "/input"                       // Rota usada no servidor
#define HTTP_RESPONSE_REDIRECT "HTTP/1.1 302 Redirect\nLocation: http://%s\n\n"

// Ações enviadas para o core 1 (controle dos servos)
#define ACAO_MOVE_SERVO_X 0
#define ACAO_MOVE_SERVO_Y 1
#define ACAO_MOVE_SERVO_Z 2

// Estrutura do servidor TCP
typedef struct TCP_SERVER_T_
{
    struct tcp_pcb *server_pcb; // Ponteiro para controle da conexão TCP
    bool complete;              // Indica se o processo foi finalizado
    ip_addr_t gw;               // Gateway IP
} TCP_SERVER_T;

// Estrutura de estado da conexão TCP
typedef struct TCP_CONNECT_STATE_T_
{
    struct tcp_pcb *pcb;    // Conexão TCP atual
    int sent_len;           // Total de bytes enviados
    char headers[256];      // Cabeçalhos HTTP
    char result[2048];      // Corpo da resposta
    int header_len;         // Tamanho dos cabeçalhos
    int result_len;         // Tamanho do corpo da resposta
    ip_addr_t *gw;          // Gateway (referência)
} TCP_CONNECT_STATE_T;

// Funções do servidor web
bool tcp_server_open(void *arg, const char *ap_name);                 // Abre servidor TCP
void tcp_server_close(TCP_SERVER_T *state);                           // Fecha servidor TCP
err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err); // Trata recebimento de dados

#endif // WEB_SERVER_H
