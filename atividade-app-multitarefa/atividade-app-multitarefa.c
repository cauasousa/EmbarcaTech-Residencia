#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Cauã de Sousa Lima
// 20251RSE.MTC0021

// Configuração do pino do buzzer
#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

// Configuração dos pinos GPIO
const uint red_pin = 13;   // Pino para o LED vermelho
const uint green_pin = 11; // Pino para o LED verde
const uint blue_pin = 12;  // Pino para o LED azul

uint32_t periodo_ms = 2000;
volatile uint32_t estado = 2;

// entre 0 e 4095
const uint ativ_baixa = 1228;    // 0 - 1228
const uint ativ_moderada = 2668; // 1228 - 2668
const uint ativ_critica = 4095;  // 2668 - 4095

// Função para atualizar os estados dos LEDs
void set_led_color(uint red_pin, uint green_pin, uint blue_pin, bool R, bool G, bool B);

// Definição de uma função para emitir um beep com duração especificada
void beep()
{
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(BUZZER_PIN, 2048);

}
void des_beep()
{
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void core1_main()
{
    while (true)
    {
        if (multicore_fifo_rvalid())
        {
            uint32_t recebido = multicore_fifo_pop_blocking();

            if (estado == 3)
            {
                // led vervemlho e buzzer
                set_led_color(red_pin, green_pin, blue_pin, 1, 0, 0);
                beep();
            }
            else if (estado == 2)
            {
                // led azul
                set_led_color(red_pin, green_pin, blue_pin, 0, 0, 1);
                des_beep();
            }
            else if (estado == 1)
            {
                // led verde
                set_led_color(red_pin, green_pin, blue_pin, 0, 1, 0);
                des_beep();
            }
        }
    }
}

int64_t temporizador_callback(alarm_id_t id, void *user_data)
{
    multicore_fifo_push_blocking(estado);

    return periodo_ms;
}

// posicao no eixo y
uint joystick();

// inicializa pwm
void setup_led_rgb();

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin);

int main()
{
    stdio_init_all();
    sleep_ms(2000); // Aguarda estabilização da USB

    adc_init();
    sleep_ms(200); // Aguarda estabilização

    adc_gpio_init(26);
    sleep_ms(200); // Aguarda estabilização

    setup_led_rgb();
    sleep_ms(200); // Aguarda estabilização

    pwm_init_buzzer(BUZZER_PIN);
    sleep_ms(200); // Aguarda estabilização

    
    multicore_launch_core1(core1_main);
    add_alarm_in_ms(periodo_ms, temporizador_callback, NULL, true); // core 0 fica responsável

    while (1)
    {

        uint posicao = joystick();

        if (ativ_moderada < posicao && ativ_critica >= posicao)
        {               // 2668 - 4095
            estado = 3; // critico
        }
        else if (ativ_baixa < posicao && ativ_moderada >= posicao)
        {               // 1228 - 2668
            estado = 2; // moderado
        }
        else if (posicao >= 0 && ativ_baixa >= posicao)
        { // 0 - 1228
            estado = 1;
        }

    }
    return 0;
}

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin)
{
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// inicializa pwm
void setup_led_rgb()
{
    gpio_init(red_pin);
    gpio_init(green_pin);
    gpio_init(blue_pin);

    gpio_set_dir(red_pin, GPIO_OUT);
    gpio_set_dir(green_pin, GPIO_OUT);
    gpio_set_dir(blue_pin, GPIO_OUT);
}

// Função para atualizar os estados dos LEDs
void set_led_color(uint red_pin, uint green_pin, uint blue_pin, bool R, bool G, bool B)
{
    gpio_put(red_pin, R);   // Configura o estado do LED vermelho
    gpio_put(green_pin, G); // Configura o estado do LED verde
    gpio_put(blue_pin, B);  // Configura o estado do LED azul
}

// posicao no eixo y
uint joystick()
{

    adc_select_input(0);
    uint adc_y_raw = adc_read();

    return adc_y_raw;
}