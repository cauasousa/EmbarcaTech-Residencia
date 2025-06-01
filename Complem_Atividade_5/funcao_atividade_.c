#include "funcao_atividade_.h"      
#include "funcoes_neopixel.h"       

int fila[TAM_FILA];                 // Fila para armazenar eventos ou dados
int inicio = 0;                     // Índice do início da fila
int fim = 0;                        // Índice do fim da fila
int quantidade = 0;                 // Quantidade de elementos na fila
int contador = 0;                   // Contador auxiliar

absolute_time_t ultimo_toque[NUM_BOTOES];          // Armazena o último tempo de toque de cada botão
const uint BOTOES[NUM_BOTOES] = {BOTAO_A, BOTAO_B, BOTAO_JOYSTICK}; // Pinos dos botões
const uint LEDS[NUM_BOTOES] = {LED_VERMELHO, LED_AZUL, LED_VERDE};  // LEDs associados a cada botão

volatile bool eventos_pendentes[NUM_BOTOES] = {false, false, false}; // Flags de eventos pendentes por botão
volatile bool estado_leds[NUM_BOTOES] = {false, false, false};       // Estados atuais dos LEDs
volatile bool core1_pronto = false;                                  // Indica se o core1 está pronto

// Função de callback para interrupções dos botões
void gpio_callback(uint gpio, uint32_t events)
{
    for (int i = 0; i < NUM_BOTOES; i++)
    {
        // Verifica se o botão gerou uma borda de descida (pressionado)
        if (gpio == BOTOES[i] && (events & GPIO_IRQ_EDGE_FALL))
        {
            multicore_fifo_push_blocking(i); // Envia o índice do botão pressionado para o outro núcleo
        }
    }
}

// Função para configurar um pino de entrada ou saída com ou sem pull-up/pull-down
void inicializar_pino(uint pino, uint direcao, bool pull_up, bool pull_down)
{
    gpio_init(pino);              // Inicializa o pino
    gpio_set_dir(pino, direcao); // Define se o pino é entrada ou saída

    if (direcao == GPIO_IN) // Se for entrada, configura os resistores de pull
    {
        if (pull_up)
        {
            gpio_pull_up(pino);     // Ativa o pull-up
        }
        else if (pull_down)
        {
            gpio_pull_down(pino);   // Ativa o pull-down
        }
        else
        {
            gpio_disable_pulls(pino); // Desativa os pull-ups e pull-downs
        }
    }
}


void tratar_eventos_leds()
{
    core1_pronto = true;

    while (true)
    {
        uint32_t id1 = multicore_fifo_pop_blocking(); // Aguarda botão pressionado

        sleep_ms(DEBOUNCE_MS);

        // Confirma se ainda está pressionado
        if (!gpio_get(BOTOES[id1]))
        {
            // Verifica se outro botão também está pressionado → ignora se sim
            bool outro_pressionado = false;

            // Esta variável vai indicar se o bottão do josytick foi pressionado ou não, se precionado vai acender os três leds rgb
            bool reset = false;

            for (int i = 0; i < NUM_BOTOES; i++)
            {
                if (i != id1 && !gpio_get(BOTOES[i]))
                {
                    outro_pressionado = true;
                    break;
                }
            }

            if (outro_pressionado)
            {
                // Espera soltar ambos
                while (!gpio_get(BOTOES[id1]))
                    tight_loop_contents();
                continue;
            }

            // Realiza logo a verificação para evitar a duplicação de printf no serial // Lógica Estabelecida
            if (id1 == 2)
            {
                reset = true; // indica que o botão do joystick foi pressionado, acender os três leds
            }

            // Ações de incremento ou decremento
            if (id1 == 0 && index_neo < LED_COUNT)
            { // BOTÃO A → incrementa
                uint8_t r = numero_aleatorio(1, 255);
                uint8_t g = numero_aleatorio(1, 255);
                uint8_t b = numero_aleatorio(1, 255);
                npAcendeLED(index_neo, r, g, b);
                index_neo++;

                // <<< Adicionado: incrementar fila
                if (quantidade < TAM_FILA)
                {
                    fila[fim] = contador++;
                    fim = (fim + 1) % TAM_FILA;
                    quantidade++;
                    imprimir_fila();
                }
            }
            else if (id1 == 1 && index_neo > 0)
            { // BOTÃO B → decrementa
                index_neo--;
                npAcendeLED(index_neo, 0, 0, 0); // apaga o LED

                // <<< Adicionado: decrementar fila
                if (quantidade > 0)
                {
                    inicio = (inicio + 1) % TAM_FILA;
                    quantidade--;
                    imprimir_fila();
                }
            }
            else if (id1 == 2 && index_neo > 0)
            { // BOTÃO B → decrementa
                index_neo = 0;

                npClear(); // apaga o LED
                npWrite(); // esscreve a modificação no matriz de leds

                // Zerando as variáveis que faz o controle do começo e tamanho da fila, além de imprimir a fila - fila estar zerada (para indicar que foi inicializada)
                if (quantidade >= 0)
                {
                    inicio = 0;
                    fim = 0;
                    contador = 0;
                    quantidade = 0;
                    index_neo = 0;
                    imprimir_fila();
                }
            }

            if (reset)
            {
                // Atualiza LEDs RGB para alto
                // todos acesos
                gpio_put(LED_VERMELHO, 1);
                gpio_put(LED_AZUL, 1);
                gpio_put(LED_VERDE, 1);
            }
            else
            {
                // Atualiza LEDs RGB externos
                gpio_put(LED_VERMELHO, (index_neo == LED_COUNT)); // todo aceso
                gpio_put(LED_AZUL, (index_neo == 0));             // tudo apagado
                gpio_put(LED_VERDE, 0);                           // opcional
            }

            // Espera botão ser solto
            while (!gpio_get(BOTOES[id1]))
            {
                tight_loop_contents();
            }
        }
    }
}

void imprimir_fila()
{
    printf("Fila [tam=%d]: ", quantidade);
    int i = inicio;
    for (int c = 0; c < quantidade; c++)
    {
        printf("%d ", fila[i]);
        i = (i + 1) % TAM_FILA;
    }
    printf("\n");
}
